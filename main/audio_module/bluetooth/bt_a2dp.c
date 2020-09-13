/*
 * @bt_a2dp.c
 * @brief Bluetooth A2DP bluetooth profile functions implementation
 *
 * @Created on: September 11, 2019
 * @Author:     Stephane Oliveira, stephane.oliveira-paredes@etu.hesge.ch
 * @Bug:        No known bugs
 * @Date:       September 13, 2019
 * @Version:    1.2.1
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2s.h"

#include "bt_a2dp.h"
#include "bt_audio_core.h"
#include "bt_avrcp.h"

#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "esp_gap_bt_api.h"
#include "esp_a2dp_api.h"
#include "nvs.h"
#include "nvs_flash.h"

#include "esp_log.h"

/* event for handler "bt_av_hdl_stack_up */
enum {
    BT_AUDIO_EVT_STACK_UP = 0,
};

static void bt_audio_hdl_stack_evt(uint16_t event, void *p_param);
static void bt_audio_a2dp_hdl_evt(uint16_t event, void *p_param);
static void bt_write_sample_buf_task(void *arg);
static void bt_write_sample_buffer(const uint8_t *data, uint32_t len);

static uint32_t s_pkt_cnt = 0;
static esp_a2d_audio_state_t s_audio_state = ESP_A2D_AUDIO_STATE_STOPPED;
static const char *s_a2d_conn_state_str[] = {"Disconnected", "Connecting", "Connected", "Disconnecting"};
static const char *s_a2d_audio_state_str[] = {"Suspended", "Stopped", "Started"};
static const char *bt_device_name;

static QueueHandle_t *bt_audio_data_queue = NULL;
static QueueHandle_t bt_raw_data_queue = NULL;
static TaskHandle_t bt_write_sample_buf_task_hdl = NULL;
static uint16_t *audio_data_buf = NULL;
static uint16_t data_buf_index = 0;
static uint16_t *audio_data_send_buf = NULL;
static uint16_t sample_num;

/**
 * @brief Initialize bluetooth audio streaming module
 * 
 * @param deviceName 	Device name to use for Bluetooth pairing
 * @note	If the device name is not specified, the default device name "SaaS-En-Boite" will be set
 */
void bt_audio_in_init(const char* deviceName, QueueHandle_t *dataQueue, uint16_t sampleNum) {
    if (deviceName) {
        bt_device_name = deviceName;
    } else {
        bt_device_name = BT_DEFAULT_DEVICE_NAME;
    }

    audio_data_buf = malloc(sampleNum * sizeof(uint16_t));
    audio_data_send_buf = malloc(sampleNum * sizeof(uint16_t));
    if (!audio_data_buf || !audio_data_send_buf) {
        ESP_LOGE(BT_AUDIO_CORE_TAG, "%s malloc failed\n", __func__);
        return;
    }

    sample_num = sampleNum;
    bt_audio_data_queue = dataQueue;
    if (!bt_audio_data_queue) {
        ESP_LOGE(BT_AUDIO_CORE_TAG, "%s dataQueue cannot be NULL\n", __func__);
        return;
    }
    

    /* Initialize NVS — it is used to store PHY calibration data */
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_BLE));
    
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    if ((err = esp_bt_controller_init(&bt_cfg)) != ESP_OK) {
        ESP_LOGE(BT_AUDIO_CORE_TAG, "%s initialize controller failed: %s\n", __func__, esp_err_to_name(err));
        return;
    }

    if ((err = esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT)) != ESP_OK) {
        ESP_LOGE(BT_AUDIO_CORE_TAG, "%s enable controller failed: %s\n", __func__, esp_err_to_name(err));
        return;
    }

    if ((err = esp_bluedroid_init()) != ESP_OK) {
        ESP_LOGE(BT_AUDIO_CORE_TAG, "%s initialize bluedroid failed: %s\n", __func__, esp_err_to_name(err));
        return;
    }

    if ((err = esp_bluedroid_enable()) != ESP_OK) {
        ESP_LOGE(BT_AUDIO_CORE_TAG, "%s enable bluedroid failed: %s\n", __func__, esp_err_to_name(err));
        return;
    }
}

/**
 * @brief Start up and initilize the Bluetooth task for the first time
 * @note Need to be called after bt_audio_in_init(). After a bt_audio_task_shut_down it is not necessary
 * to recall this function, just call bt_audio_task_start_up.
 */
void bt_audio_init_start_up(void) {
    /* create application task */
    bt_audio_task_start_up();
    bt_audio_start_up();
}

void bt_audio_start_up(void) {
    // Launch start up event
    bt_audio_work_dispatch(bt_audio_hdl_stack_evt, BT_AUDIO_EVT_STACK_UP, NULL, 0);

    bt_raw_data_queue = xQueueCreate(10, sizeof(bt_raw_data_t));

    xTaskCreate(bt_write_sample_buf_task, "bt_write_sample_buffer", 2048, NULL, configMAX_PRIORITIES - 1,
        &bt_write_sample_buf_task_hdl);
}

void bt_audio_shut_down(void) {
    bt_audio_task_shut_down();

    if (bt_write_sample_buf_task_hdl) {
        vTaskDelete(bt_write_sample_buf_task_hdl);
    }

    if (bt_raw_data_queue) {
        vQueueDelete(bt_raw_data_queue);
    }
}
/*==============================================================================*
                                A2DP CALLBACKS
 *==============================================================================*/

/**
 * @brief Callback function for A2DP sink
 * 
 * @param event 		A2DP callback event
 * @param param 		A2DP callback parameter
 */
void bt_a2dp_cb(esp_a2d_cb_event_t event, esp_a2d_cb_param_t *param) {
    switch (event) {
    case ESP_A2D_CONNECTION_STATE_EVT:
    case ESP_A2D_AUDIO_STATE_EVT:
    case ESP_A2D_AUDIO_CFG_EVT: {
        bt_audio_work_dispatch(bt_audio_a2dp_hdl_evt, event, param, sizeof(esp_a2d_cb_param_t));
        break;
    }
    default:
        ESP_LOGE(BT_A2DP_TAG, "Invalid A2DP event: %d", event);
        break;
    }
}

/**
 * @brief Callback function for A2DP data receive
 * 
 * @param data 			A2DP data receive
 * @param len 			A2DP data length
 */
void bt_a2dp_data_cb(const uint8_t *data, uint32_t len) {
    if (ESP_A2D_AUDIO_STATE_STARTED == s_audio_state) {
        bt_raw_data_t rawData = {
            .data_buf = data,
            .length = len
        };

        xQueueSend(bt_raw_data_queue, &rawData, portMAX_DELAY);
    }

    if (++s_pkt_cnt % 100 == 0) {
        ESP_LOGI(BT_A2DP_TAG, "Audio packet count %u", s_pkt_cnt);
    }
}

/**
 * @brief Task handler that writes the A2DP raw data into samples array
 * 
 * @param arg       Not specified
 */
static void bt_write_sample_buf_task(void *arg) {
    bt_raw_data_t raw_data;

    for (;;) {
        if (pdTRUE == xQueueReceive(bt_raw_data_queue, &raw_data, portMAX_DELAY)) {
            bt_write_sample_buffer(raw_data.data_buf, raw_data.length);
        }
    }
    
}

/**
 * @brief Write data from Bluetooth buffer to sample buffer and send it when it's full.
 * 
 * @param data          A2DP data buffer
 * @param len           A2DP data length
 */
static void bt_write_sample_buffer(const uint8_t *data, uint32_t len) {
    //loop on every first byte of audio left channel
    for (uint16_t i = 0; i < len; i+=4) {
        audio_data_buf[data_buf_index] = (uint16_t) ((data[i] << 8) | data[i+1]);
        if (data_buf_index++ >= (sample_num - 1)) {
            memcpy(audio_data_send_buf, audio_data_buf, (sizeof(uint16_t) * sample_num));
            //ESP_LOGI(BT_A2DP_TAG, "Audio samples sent");
            xQueueSend(*bt_audio_data_queue, audio_data_send_buf, portMAX_DELAY);
            data_buf_index = 0;
        }
    }
}

/**
 * @brief Callback function for Bluetooth GAP (bluetooth connexion)
 * 
 * @param event 		Bluetooth GAP event
 * @param param 		Callback params pointer
 */
void bt_audio_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param) {
    switch (event) {
    case ESP_BT_GAP_AUTH_CMPL_EVT: {
        if (param->auth_cmpl.stat == ESP_BT_STATUS_SUCCESS) {
            ESP_LOGI(BT_A2DP_TAG, "authentication success: %s", param->auth_cmpl.device_name);
            esp_log_buffer_hex(BT_A2DP_TAG, param->auth_cmpl.bda, ESP_BD_ADDR_LEN);
        } else {
            ESP_LOGE(BT_A2DP_TAG, "authentication failed, status:%d", param->auth_cmpl.stat);
        }
        break;
    }

#if (CONFIG_BT_SSP_ENABLED == true)
    case ESP_BT_GAP_CFM_REQ_EVT:
        ESP_LOGI(BT_A2DP_TAG, "ESP_BT_GAP_CFM_REQ_EVT Please compare the numeric value: %d", param->cfm_req.num_val);
        esp_bt_gap_ssp_confirm_reply(param->cfm_req.bda, true);
        break;
    case ESP_BT_GAP_KEY_NOTIF_EVT:
        ESP_LOGI(BT_A2DP_TAG, "ESP_BT_GAP_KEY_NOTIF_EVT passkey:%d", param->key_notif.passkey);
        break;
    case ESP_BT_GAP_KEY_REQ_EVT:
        ESP_LOGI(BT_A2DP_TAG, "ESP_BT_GAP_KEY_REQ_EVT Please enter passkey!");
        break;
#endif

    default: {
        ESP_LOGI(BT_A2DP_TAG, "event: %d", event);
        break;
    }
    }
    return;
}

/*==============================================================================*
                                STATIC HANDLERS
 *==============================================================================*/

/**
 * @brief Bluetooth A2DP event handler
 * 
 * @param event 		A2DP event type
 * @param p_param 		Event params pointer
 */
static void bt_audio_a2dp_hdl_evt(uint16_t event, void *p_param) {
    ESP_LOGD(BT_A2DP_TAG, "%s evt %d", __func__, event);
    esp_a2d_cb_param_t *a2d = NULL;
    switch (event) {
    case ESP_A2D_CONNECTION_STATE_EVT: {
        a2d = (esp_a2d_cb_param_t *)(p_param);
        uint8_t *bda = a2d->conn_stat.remote_bda;
        ESP_LOGI(BT_A2DP_TAG, "A2DP connection state: %s, [%02x:%02x:%02x:%02x:%02x:%02x]",
             s_a2d_conn_state_str[a2d->conn_stat.state], bda[0], bda[1], bda[2], bda[3], bda[4], bda[5]);
        if (a2d->conn_stat.state == ESP_A2D_CONNECTION_STATE_DISCONNECTED) {
            esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
            //bt_i2s_task_shut_down();
        } else if (a2d->conn_stat.state == ESP_A2D_CONNECTION_STATE_CONNECTED){
            esp_bt_gap_set_scan_mode(ESP_BT_NON_CONNECTABLE, ESP_BT_NON_DISCOVERABLE);
            //bt_i2s_task_start_up();
        }
        break;
    }
    case ESP_A2D_AUDIO_STATE_EVT: {
        a2d = (esp_a2d_cb_param_t *)(p_param);
        ESP_LOGI(BT_A2DP_TAG, "A2DP audio state: %s", s_a2d_audio_state_str[a2d->audio_stat.state]);
        s_audio_state = a2d->audio_stat.state;
        if (ESP_A2D_AUDIO_STATE_STARTED == a2d->audio_stat.state) {
            s_pkt_cnt = 0;
        }
        break;
    }
    case ESP_A2D_AUDIO_CFG_EVT: {
        a2d = (esp_a2d_cb_param_t *)(p_param);
        ESP_LOGI(BT_A2DP_TAG, "A2DP audio stream configuration, codec type %d", a2d->audio_cfg.mcc.type);
        // for now only SBC stream is supported
        if (a2d->audio_cfg.mcc.type == ESP_A2D_MCT_SBC) {
            int sample_rate = 16000;
            char oct0 = a2d->audio_cfg.mcc.cie.sbc[0];
            if (oct0 & (0x01 << 6)) {
                sample_rate = 32000;
            } else if (oct0 & (0x01 << 5)) {
                sample_rate = 44100;
            } else if (oct0 & (0x01 << 4)) {
                sample_rate = 48000;
            }
            //i2s_set_clk(0, sample_rate, 16, 2);

            ESP_LOGI(BT_A2DP_TAG, "Configure audio player %x-%x-%x-%x",
                     a2d->audio_cfg.mcc.cie.sbc[0],
                     a2d->audio_cfg.mcc.cie.sbc[1],
                     a2d->audio_cfg.mcc.cie.sbc[2],
                     a2d->audio_cfg.mcc.cie.sbc[3]);
            ESP_LOGI(BT_A2DP_TAG, "Audio player configured, sample rate=%d", sample_rate);
        }
        break;
    }
    default:
        ESP_LOGE(BT_A2DP_TAG, "%s unhandled evt %d", __func__, event);
        break;
    }
}

/**
 * @brief Bluetooth Stack event handler
 * 
 * @param event 		Stack event
 * @param p_param 		Event params pointer
 */
static void bt_audio_hdl_stack_evt(uint16_t event, void *p_param) {
    ESP_LOGD(BT_A2DP_TAG, "%s evt %d", __func__, event);
    switch (event) {
    case BT_AUDIO_EVT_STACK_UP: {
        /* set up device name */
        esp_bt_dev_set_device_name(bt_device_name);
        esp_bt_gap_register_callback(bt_audio_gap_cb);

        /* initialize AVRCP controller */
        esp_avrc_ct_init();
        esp_avrc_ct_register_callback(bt_avrc_ct_cb);
        /* initialize AVRCP target */
        assert(esp_avrc_tg_init() == ESP_OK);
        esp_avrc_tg_register_callback(bt_avrc_tg_cb);

        /* initialize A2DP sink */
        esp_a2d_register_callback(&bt_a2dp_cb);
        esp_a2d_sink_register_data_callback(bt_a2dp_data_cb);
        esp_a2d_sink_init();

        /* set discoverable and connectable mode, wait to be connected */
        esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
        break;
    }
    default:
        ESP_LOGE(BT_A2DP_TAG, "%s unhandled evt %d", __func__, event);
        break;
    }
}