/*
 * @main.c
 * @brief Main entry of SaaS-En-Boite project
 *
 * @Created on: August 29, 2019
 * @Author:     Nicholas Hamilton, nicholas.hamilton-james@etu.hesge.ch
 * @Author:     Stephane Oliveira, stephane.oliveira-paredes@etu.hesge.ch
 * @Author:     David Nogueiras, david.nogueira1@etu.hesge.ch
 * @Bug:        No known bugs
 * @Date:       September 12, 2019
 * @Version:    1.0.2
 */
#include <stdbool.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "audio_module/bluetooth/bt_audio_core.h"
#include "audio_module/bluetooth/bt_a2dp.h"

#include "dmx_module/dmx_task.h"

#include "beat_detection_module/beat_detector_task.h"

#define SAMPLE_NUM (64)

#define MAIN_APP_TAG		"MAIN_APP"

typedef uint16_t audio_sample_t[SAMPLE_NUM];
xQueueHandle dmxQueue;

xQueueHandle audio_samples_queue;

void app_main(void) {
	audio_samples_queue = xQueueCreate(10, sizeof(audio_sample_t));
    dmxQueue = xQueueCreate( 10, sizeof(uint8_t));
	/* ==== Example code to use audio bluetooth task ====*/
	//bt_audio_in_init("SaaS-En-Boite");
	//bt_audio_init_start_up();

    init_beat_detector_t(&audio_samples_queue, &dmxQueue);
    create_beat_detector_tasks();
	bt_audio_in_init("SaaS-En-Boite", &audio_samples_queue, SAMPLE_NUM);
	bt_audio_init_start_up();
    createDmxTasks(dmxQueue);
}

