/*
 * @bt_a2dp.h
 * @brief Bluetooth A2DP bluetooth profile functions/types/macros definition
 *
 * @Created on: September 11, 2019
 * @Author:     Stephane Oliveira, stephane.oliveira-paredes@etu.hesge.ch
 * @Bug:        No known bugs
 * @Date:       September 12, 2019
 * @Version:    1.2
 */
#ifndef __BT_APP_AV_H__
#define __BT_APP_AV_H__

#include <stdint.h>
#include "freertos/queue.h"
#include "esp_a2dp_api.h"
#include "esp_avrc_api.h"

#define BT_A2DP_TAG					"BT_A2DP"
#define BT_DEFAULT_DEVICE_NAME		"SaaS-En-Boite"

typedef struct {
	const uint8_t *data_buf;
	uint32_t length;
} bt_raw_data_t;

/**
 * @brief Initialize bluetooth audio streaming module
 * 
 * @param deviceName 	Device name to use for Bluetooth pairing
 * @note	If the device name is not specified, the default device name "SaaS-En-Boite" will be set.
 * @param dataQueue 	Data queue pointer
 * @param sampleNum 	Number of samples to send with xQueue
 */
void bt_audio_in_init(const char* deviceName, QueueHandle_t *dataQueue, uint16_t sampleNum);

/**
 * @brief Start up and initilize the Bluetooth task for the first time
 * @note Need to be called after bt_audio_in_init(). After a bt_audio_task_shut_down it is not necessary
 * to recall this function, just call bt_audio_task_start_up.
 */
void bt_audio_init_start_up(void);

void bt_audio_start_up(void);


/**
 * @brief Callback function for A2DP sink
 * 
 * @param event 		A2DP callback event
 * @param param 		A2DP callback parameter
 */
void bt_a2dp_cb(esp_a2d_cb_event_t event, esp_a2d_cb_param_t *param);

/**
 * @brief Callback function for A2DP data receive
 * 
 * @param data 			A2DP data receive
 * @param len 			A2DP data length
 */
void bt_a2dp_data_cb(const uint8_t *data, uint32_t len);

#endif /* __BT_APP_AV_H__*/