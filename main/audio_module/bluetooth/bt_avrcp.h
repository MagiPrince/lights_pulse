/*
 * @bt_avrcp.h
 * @brief Bluetooth AVRC Profile functions/types/macros definition
 *
 * @Created on: September 12, 2019
 * @Author:     Stephane Oliveira, stephane.oliveira-paredes@etu.hesge.ch
 * @Bug:        No known bugs
 * @Date:       September 12, 2019
 * @Version:    1.0
 */

#ifndef __BT_AVRCP_H__
#define __BT_AVRCP_H__

#include <stdint.h>

#include "esp_avrc_api.h"


#define BT_AVRCP_TAG		"BT_AVRCP"


void bt_avrc_tg_cb(esp_avrc_tg_cb_event_t event, esp_avrc_tg_cb_param_t *param);
void bt_avrc_ct_cb(esp_avrc_ct_cb_event_t event, esp_avrc_ct_cb_param_t *param);


#endif