/*
 * @bt_audio_core.h
 * @brief Bluetooth audio IN core module functions/types/macros definition
 *
 * @Created on: September 10, 2019
 * @Author:     Stephane Oliveira, stephane.oliveira-paredes@etu.hesge.ch
 * @Bug:        No known bugs
 * @Date:       September 11, 2019
 * @Version:    1.1
 */
#ifndef __BT_AUDIO_IN_H__
#define __BT_AUDIO_IN_H__
#include <stdint.h>

#include "esp_a2dp_api.h"

#define BT_AUDIO_CORE_TAG		("BT_AUDIO_CORE")

/**
 * @brief     handler for the dispatched work
 */
typedef void (* bt_audio_cb_t) (uint16_t event, void *param);

/* message to be sent */
typedef struct {
    uint16_t             sig;      /*!< signal to bt_audio_task */
    uint16_t             event;    /*!< message event id */
    bt_audio_cb_t          cb;       /*!< context switch callback */
    void                 *param;   /*!< parameter area needs to be last */
} bt_audio_msg_t;

/**
 * @brief     parameter deep-copy function to be customized
 */
typedef void (* bt_audio_copy_cb_t) (bt_audio_msg_t *msg, void *p_dest, void *p_src);

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
bool bt_audio_work_dispatch(bt_audio_cb_t p_cback, uint16_t event, void *p_params, int param_len);

/**
 * @brief Starts Bluetooth audio task
 */
void bt_audio_task_start_up(void);

/**
 * @brief Shut down Bluetooth audio task
 */
void bt_audio_task_shut_down(void);

void bt_i2s_task_start_up(void);

void bt_i2s_task_shut_down(void);

size_t write_ringbuf(const uint8_t *data, size_t size);

#endif