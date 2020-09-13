/*
 * @bt_audio_core.c
 * @brief Bluetooth audio IN core module functions implementation
 *
 * @Created on: September 10, 2019
 * @Author:     Stephane Oliveira, stephane.oliveira-paredes@etu.hesge.ch
 * @Bug:        No known bugs
 * @Date:       September 11, 2019
 * @Version:    1.1
 */
#include <string.h>

#include "freertos/xtensa_api.h"
#include "freertos/FreeRTOSConfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/ringbuf.h"

#include "driver/i2s.h"

#include "bt_audio_core.h"

#include "esp_log.h"
#include "esp_bt.h"
#include "esp_gap_bt_api.h"
#include "esp_err.h"

#define bt_audio_EVT_STACK_UP 		(0)
#define bt_audio_SIG_WORK_DISPATCH	(0x01)

static QueueHandle_t bt_audio_task_queue = NULL;
static TaskHandle_t bt_audio_task_handle = NULL;
static TaskHandle_t bt_i2s_task_handle = NULL;
static RingbufHandle_t ringbuf_i2s = NULL;

static void bt_audio_task_handler(void *arg);
static bool bt_audio_send_msg(bt_audio_msg_t *msg);
static void bt_audio_work_dispatched(bt_audio_msg_t *msg);

/*==============================================================================*
						Bluetooth Static Functions
 *==============================================================================*/

/**
 * @brief Sends a message to Bluetooth audio task
 * 
 * @param msg 		Message to send to the Bluetooth audio task queue
 * @return true 
 * @return false 
 */
static bool bt_audio_send_msg(bt_audio_msg_t *msg) {
	if (msg == NULL) {
		return false;
	}

	if (xQueueSend(bt_audio_task_queue, msg, 10 / portTICK_RATE_MS) != pdTRUE) {
		ESP_LOGE(BT_AUDIO_CORE_TAG, "%s xQueue send failed", __func__);
		return false;
	}
	return true;
}

/**
 * @brief Call audio_msg callback if set
 * 
 * @param msg 		Bluetooth Audio message
 */
static void bt_audio_work_dispatched(bt_audio_msg_t *msg) {
	if (msg->cb) {
		msg->cb(msg->event, msg->param);
	}
}

/**
 * @brief Bluetooth audio task Handler
 * 
 * @param arg 		Argument not necessary for this task
 */
static void bt_audio_task_handler(void *arg) {
	bt_audio_msg_t msg;
	for (;;) {
		if (pdTRUE == xQueueReceive(bt_audio_task_queue, &msg, (portTickType)portMAX_DELAY)) {
			ESP_LOGD(BT_AUDIO_CORE_TAG, "%s, sig 0x%x, 0x%x", __func__, msg.sig, msg.event);
			switch (msg.sig) {
			case bt_audio_SIG_WORK_DISPATCH:
				bt_audio_work_dispatched(&msg);
				break;
			default:
				ESP_LOGW(BT_AUDIO_CORE_TAG, "%s, unhandled sig: %d", __func__, msg.sig);
				break;
			} // switch (msg.sig)

			if (msg.param) {
				free(msg.param);
			}
		}
	}
}

/*==============================================================================*
								Bluetooth functions
 *==============================================================================*/

/**
 * @brief Dispatched work for the Bluetooth audio task.
 * @note This function creates a bt_audio_msg_t with the params given and send it by calling bt_audio_send_msg 
 * 
 * @param p_cback 		Event pointer callback
 * @param event 		Event type
 * @param p_params 		Callback params pointer
 * @param param_len 	Params length
 * @return true 		If the bt_audio_msg was successfully created and sent.
 * @return false 		If an error occured (while creating or sending the bt_audio_msg)
 */
bool bt_audio_work_dispatch(bt_audio_cb_t p_cback, uint16_t event, void *p_params, int param_len) {
	ESP_LOGD(BT_AUDIO_CORE_TAG, "%s event 0x%x, param len %d", __func__, event, param_len);

	bt_audio_msg_t msg;
	memset(&msg, 0, sizeof(bt_audio_msg_t));

	msg.sig = bt_audio_SIG_WORK_DISPATCH;
	msg.event = event;
	msg.cb = p_cback;

	if (param_len == 0) {
		return bt_audio_send_msg(&msg);
	} else if (p_params && param_len > 0) {
		if ((msg.param = malloc(param_len)) != NULL) {
			memcpy(msg.param, p_params, param_len);

			return bt_audio_send_msg(&msg);
		}
	}

	return false;
}

/**
 * @brief Starts Bluetooth audio task
 */
void bt_audio_task_start_up(void) {
	bt_audio_task_queue = xQueueCreate(10, sizeof(bt_audio_msg_t));
	xTaskCreate(bt_audio_task_handler, "BtAppT", 4096, NULL, configMAX_PRIORITIES, &bt_audio_task_handle);
	return;
}

/**
 * @brief Shut down Bluetooth audio task 
 */
void bt_audio_task_shut_down(void) {
	if (bt_audio_task_handle) {
		vTaskDelete(bt_audio_task_handle);
		bt_audio_task_handle = NULL;
	}
	if (bt_audio_task_queue) {
		vQueueDelete(bt_audio_task_queue);
		bt_audio_task_queue = NULL;
	}
}

/*==============================================================================*
							***I2S related functions***
									Not used yet
 *==============================================================================*/

static void bt_i2s_task_handler(void *arg) {
	uint8_t *data = NULL;
	size_t item_size = 0;
	size_t bytes_written = 0;

	for (;;) {
		data = (uint8_t *)xRingbufferReceive(ringbuf_i2s, &item_size, (portTickType)portMAX_DELAY);
		if (item_size != 0){
			i2s_write(0, data, item_size, &bytes_written, portMAX_DELAY);
			vRingbufferReturnItem(ringbuf_i2s,(void *)data);
		}
	}
}

void bt_i2s_task_start_up(void) {
	ringbuf_i2s = xRingbufferCreate(8 * 1024, RINGBUF_TYPE_BYTEBUF);
	if(ringbuf_i2s == NULL){
		return;
	}

	xTaskCreate(bt_i2s_task_handler, "AudioOut_I2S_Task", 1024, NULL, configMAX_PRIORITIES - 3, &bt_i2s_task_handle);
	return;
}

void bt_i2s_task_shut_down(void) {
	if (bt_i2s_task_handle) {
		vTaskDelete(bt_i2s_task_handle);
		bt_i2s_task_handle = NULL;
	}

	if (ringbuf_i2s) {
		vRingbufferDelete(ringbuf_i2s);
		ringbuf_i2s = NULL;
	}
}

size_t write_ringbuf(const uint8_t *data, size_t size) {
	BaseType_t done = xRingbufferSend(ringbuf_i2s, (void *)data, size, (portTickType)portMAX_DELAY);
	if(done){
		return size;
	} else {
		return 0;
	}
}