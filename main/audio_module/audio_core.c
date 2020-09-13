/*
 * @audio_core.c
 * @brief Audio module core library
 *
 * @Created on: September 3, 2019
 * @Author:     Stephane Oliveira, stephane.oliveira-paredes@etu.hesge.ch
 * @Bug:        No known bugs
 * @Date:       September 4, 2019
 * @Version:    1.1
 */
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/timer.h"

#include "audio_core.h"

#define AUDIO_IN_MISO_LEFT		GPIO_NUM_25
#define AUDIO_IN_MISO_RIGHT		GPIO_NUM_26
#define AUDIO_IN_CLK			GPIO_NUM_19
#define AUDIO_IN_SS				GPIO_NUM_22
#define AUDIO_MIC_SS			GPIO_NUM_23

#define AUDIO_SPI_CLK			SPI_MASTER_FREQ_20M
#define AUDIO_SPI_LEN			(18) //16bits audio data length

#define AUDIO_TIMER_IDX			(TIMER_0)
#define AUDIO_TIMER_DIV			(8)
#define AUDIO_TIMER_SCALE		(TIMER_BASE_CLK / AUDIO_TIMER_DIV)

static const char* TAG = "AudioCoreModule";

static spi_device_handle_t spi;

xQueueHandle timer_sampler_queue;

static inline uint32_t audio_get_data_in(void);

/**
 * @brief ISR function for timer group 0
 * 
 * @param para 			Timer_idx generating the interruption
 */
void IRAM_ATTR timer_group0_isr(void *param) {
	timer_intr_t timer_intr = timer_group_intr_get_in_isr(TIMER_GROUP_0);

	if (timer_intr & TIMER_INTR_T0) {
		ESP_LOGD(TAG, "Clear l'interrupt");
		timer_group_intr_clr_in_isr(TIMER_GROUP_0, TIMER_0);
	} if (timer_intr & TIMER_INTR_T1) {
		timer_group_intr_clr_in_isr(TIMER_GROUP_0, TIMER_1);
	}

	timer_group_enable_alarm_in_isr(TIMER_GROUP_0, AUDIO_TIMER_IDX);
	uint8_t data = 0;
    /* Now just send the event data back to the main program task */
    xQueueSendFromISR(timer_sampler_queue, &data, NULL);
}

/**
 * @brief FreeRtos task getting audio in data
 * 
 * @param pvParameters 		Pointer of audio task params/config
 */
void vTask_Audio_In(void *pvParameters) {
	audio_task_params_t *params = (audio_task_params_t*) pvParameters;
	uint16_t samples[params->sample_len];
	uint8_t i = 0;

	/*TickType_t xLastWakeTime;
 	const TickType_t xFrequency = 1;
	xLastWakeTime = xTaskGetTickCount();*/

	for (;;){
		if (i <= 0) {
			samples[i] = audio_get_data_in();//Getting data from SPI
			i++;
			uint8_t msg;
			xQueueReceive(timer_sampler_queue, &msg, portMAX_DELAY);
		} else {
			i = 0;
			ESP_LOGI(TAG, "Samples set ready.");
			memcpy(params->samples, &samples, params->sample_len);// copy all samples in pointer given by params
			params->samplesReady = true;
		}
	}
	
}

/*======================================================================
							Audio init functions
 =======================================================================*/
 
/**
 * @brief Initialize audio timer for sampling de data in
 * 
 * @param frequency 		Audio sampling rate frequency
 */
static void audio_timer_init(uint32_t frequency) {
	ESP_LOGI(TAG, "Initializing Audio IN Timer sampler");
	timer_config_t config;
	config.divider = AUDIO_TIMER_DIV;
	config.counter_dir = TIMER_COUNT_UP;
	config.counter_en = TIMER_PAUSE;
	config.alarm_en = TIMER_ALARM_EN;
	config.intr_type = TIMER_INTR_LEVEL;
	config.auto_reload = true;
	timer_init(TIMER_GROUP_0, AUDIO_TIMER_IDX, &config);

	timer_set_counter_value(TIMER_GROUP_0, AUDIO_TIMER_IDX, 0x00000000ULL);

	/* Configure the alarm value and the interrupt on alarm. */
	uint32_t dada = (uint32_t) (5);
	ESP_LOGI(TAG, "Alarm counter value: %d", dada);
	timer_set_alarm_value(TIMER_GROUP_0, AUDIO_TIMER_IDX, (uint64_t)dada);
	timer_enable_intr(TIMER_GROUP_0, AUDIO_TIMER_IDX);
	timer_isr_register(TIMER_GROUP_0, AUDIO_TIMER_IDX, timer_group0_isr,
		(void *) AUDIO_TIMER_IDX, ESP_INTR_FLAG_IRAM, NULL);
}

/**
 * @brief Initialize and configure audio in SPI
 * 
 */
static void audio_spi_init(void) {
	spi_bus_config_t buscfg = {
		.miso_io_num = AUDIO_IN_MISO_LEFT,
		.mosi_io_num = -1,
		.sclk_io_num = AUDIO_IN_CLK,
		.quadwp_io_num = -1,
		.quadhd_io_num = -1
	};
	spi_device_interface_config_t audioInConfig = {
		.clock_speed_hz = AUDIO_SPI_CLK,           //Clock out at 20 MHz
		.mode = 0,                                //SPI mode 0
		.spics_io_num = AUDIO_IN_SS,               //CS pin
		.duty_cycle_pos = 128,
		.queue_size = 10
	};
	/*
	spi_device_interface_config_t micConfig = {
		.clock_speed_hz = AUDIO_SPI_CLK,           //Clock out at 20 MHz
		.mode = 0,                                //SPI mode 0
		.spics_io_num = AUDIO_MIC_SS,             //CS pin
		.duty_cycle_pos = 128,
		.queue_size = 10
	};*/
	
	ESP_LOGI(TAG, "Initializing SPI Bus");
	ESP_ERROR_CHECK(//Initialize the SPI bus
		spi_bus_initialize(HSPI_HOST, &buscfg, 1)
	);

	ESP_LOGI(TAG, "Attaching SPI bus to Pmod Audio IN");
	ESP_ERROR_CHECK(//Attach bus to Pmod Audio IN
		spi_bus_add_device(HSPI_HOST, &audioInConfig, &spi)
	);
}

/**
 * @brief Get audio data via SPI from PMOD Audio In or PMOD Mic
 * 
 * @return uint32_t 	Audio sample data
 */
static inline uint32_t audio_get_data_in(void) {
	spi_transaction_t t;
	memset(&t, 0, sizeof(t));
	t.length = AUDIO_SPI_LEN;
	t.flags = SPI_TRANS_USE_RXDATA;
	t.user = (void*)1;
	esp_err_t ret = spi_device_polling_transmit(spi, &t);
	assert( ret == ESP_OK );
	ESP_LOGI(TAG, "RX_data: %#X", *(uint32_t*)t.rx_data);
	return *(uint32_t*)t.rx_data;
}

/**
 * @brief Initialize audio in module
 * 
 */
void audio_in_init(TaskHandle_t *audioHandle, audio_task_params_t *params) {
	ESP_LOGI(TAG, "Initializing Audio IN module");
	audio_spi_init();
	timer_sampler_queue = xQueueCreate(5, sizeof(uint8_t));
	audio_timer_init(44100);
}

/**
 * @brief Start audio in sampling
 * 
 */
void audio_sample_start(void) {
	ESP_LOGI(TAG, "Starting Audio IN Sampling");
	timer_start(TIMER_GROUP_0, AUDIO_TIMER_IDX);
}

/**
 * @brief Stop audio in sampling
 * 
 */
void audio_sample_stop(void) {
	ESP_LOGI(TAG, "Stopping Audio IN Sampling");
	timer_pause(TIMER_GROUP_0, AUDIO_TIMER_IDX);
}