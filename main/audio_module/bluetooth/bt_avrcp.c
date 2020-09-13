/*
 * @bt_avrcp.c
 * @brief Bluetooth AVRC Profile functions implementation
 *
 * @Created on: September 12, 2019
 * @Author:     Stephane Oliveira, stephane.oliveira-paredes@etu.hesge.ch
 * @Bug:        No known bugs
 * @Date:       September 12, 2019
 * @Version:    1.0
 */
#include <stdbool.h>

#include "esp_log.h"

#include "bt_avrcp.h"
#include "bt_audio_core.h"

#include "sys/lock.h"



static void bt_avrc_ct_evt_hdl(uint16_t event, void *p_param);
static void bt_avrc_tg_evt_hdl(uint16_t event, void *p_param);

static esp_avrc_rn_evt_cap_mask_t s_avrc_peer_rn_cap;
static uint8_t s_volume = 0;
static bool s_volume_notify;
static _lock_t s_volume_lock;


static void volume_set_by_controller(uint8_t volume) {
    ESP_LOGI(BT_AVRCP_TAG, "Volume is set by remote controller %d%%\n", (uint32_t)volume * 100 / 0x7f);
    _lock_acquire(&s_volume_lock);
    s_volume = volume;
    _lock_release(&s_volume_lock);
}

void bt_avrc_ct_cb(esp_avrc_ct_cb_event_t event, esp_avrc_ct_cb_param_t *param) {
    switch (event) {
    case ESP_AVRC_CT_METADATA_RSP_EVT:
        //bt_app_alloc_meta_buffer(param);
        /* fall through */
    case ESP_AVRC_CT_CONNECTION_STATE_EVT:
    case ESP_AVRC_CT_PASSTHROUGH_RSP_EVT:
    case ESP_AVRC_CT_CHANGE_NOTIFY_EVT:
    case ESP_AVRC_CT_REMOTE_FEATURES_EVT:
    case ESP_AVRC_CT_GET_RN_CAPABILITIES_RSP_EVT: {
        bt_audio_work_dispatch(bt_avrc_ct_evt_hdl, event, param, sizeof(esp_avrc_ct_cb_param_t));
        break;
    }
    default:
        ESP_LOGE(BT_AVRCP_TAG, "Invalid AVRC event: %d", event);
        break;
    }
}

void bt_avrc_tg_cb(esp_avrc_tg_cb_event_t event, esp_avrc_tg_cb_param_t *param) {
    switch (event) {
    case ESP_AVRC_TG_CONNECTION_STATE_EVT:
    case ESP_AVRC_TG_REMOTE_FEATURES_EVT:
    case ESP_AVRC_TG_PASSTHROUGH_CMD_EVT:
    case ESP_AVRC_TG_SET_ABSOLUTE_VOLUME_CMD_EVT:
    case ESP_AVRC_TG_REGISTER_NOTIFICATION_EVT:
        bt_audio_work_dispatch(bt_avrc_tg_evt_hdl, event, param, sizeof(esp_avrc_tg_cb_param_t));
        break;
    default:
        ESP_LOGE(BT_AVRCP_TAG, "Invalid AVRC event: %d", event);
        break;
    }
}

static void bt_avrc_ct_evt_hdl(uint16_t event, void *p_param) {
    ESP_LOGD(BT_AVRCP_TAG, "%s evt %d", __func__, event);
    esp_avrc_ct_cb_param_t *rc = (esp_avrc_ct_cb_param_t *)(p_param);
    switch (event) {
    case ESP_AVRC_CT_CONNECTION_STATE_EVT: {
        uint8_t *bda = rc->conn_stat.remote_bda;
        ESP_LOGI(BT_AVRCP_TAG, "AVRC conn_state evt: state %d, [%02x:%02x:%02x:%02x:%02x:%02x]",
                 rc->conn_stat.connected, bda[0], bda[1], bda[2], bda[3], bda[4], bda[5]);
        break;
    }
    case ESP_AVRC_CT_PASSTHROUGH_RSP_EVT: {
        ESP_LOGI(BT_AVRCP_TAG, "AVRC passthrough rsp: key_code 0x%x, key_state %d", rc->psth_rsp.key_code, rc->psth_rsp.key_state);
        break;
    }
    case ESP_AVRC_CT_METADATA_RSP_EVT: {
        ESP_LOGI(BT_AVRCP_TAG, "AVRC metadata rsp: attribute id 0x%x, %s", rc->meta_rsp.attr_id, rc->meta_rsp.attr_text);
        free(rc->meta_rsp.attr_text);
        break;
    }
    case ESP_AVRC_CT_CHANGE_NOTIFY_EVT: {
        ESP_LOGI(BT_AVRCP_TAG, "AVRC event notification: %d", rc->change_ntf.event_id);
        break;
    }
    case ESP_AVRC_CT_REMOTE_FEATURES_EVT: {
        ESP_LOGI(BT_AVRCP_TAG, "AVRC remote features %x, TG features %x", rc->rmt_feats.feat_mask, rc->rmt_feats.tg_feat_flag);
        break;
    }
    case ESP_AVRC_CT_GET_RN_CAPABILITIES_RSP_EVT: {
        ESP_LOGI(BT_AVRCP_TAG, "remote rn_cap: count %d, bitmask 0x%x", rc->get_rn_caps_rsp.cap_count,
                 rc->get_rn_caps_rsp.evt_set.bits);
        s_avrc_peer_rn_cap.bits = rc->get_rn_caps_rsp.evt_set.bits;
        break;
    }
    default:
        ESP_LOGE(BT_AVRCP_TAG, "%s unhandled evt %d", __func__, event);
        break;
    }
}

static void bt_avrc_tg_evt_hdl(uint16_t event, void *p_param) {
    ESP_LOGD(BT_AVRCP_TAG, "%s evt %d", __func__, event);
    esp_avrc_tg_cb_param_t *rc = (esp_avrc_tg_cb_param_t *)(p_param);
    switch (event) {
    case ESP_AVRC_TG_CONNECTION_STATE_EVT: {
        uint8_t *bda = rc->conn_stat.remote_bda;
        ESP_LOGI(BT_AVRCP_TAG, "AVRC conn_state evt: state %d, [%02x:%02x:%02x:%02x:%02x:%02x]",
                 rc->conn_stat.connected, bda[0], bda[1], bda[2], bda[3], bda[4], bda[5]);
        break;
    }
    case ESP_AVRC_TG_PASSTHROUGH_CMD_EVT: {
        ESP_LOGI(BT_AVRCP_TAG, "AVRC passthrough cmd: key_code 0x%x, key_state %d", rc->psth_cmd.key_code, rc->psth_cmd.key_state);
        break;
    }
    case ESP_AVRC_TG_SET_ABSOLUTE_VOLUME_CMD_EVT: {
        volume_set_by_controller(rc->set_abs_vol.volume);
        ESP_LOGI(BT_AVRCP_TAG, "AVRC set absolute volume: %d%%", (int)rc->set_abs_vol.volume * 100/ 0x7f);
        break;
    }
    case ESP_AVRC_TG_REGISTER_NOTIFICATION_EVT: {
        ESP_LOGI(BT_AVRCP_TAG, "AVRC register event notification: %d, param: 0x%x", rc->reg_ntf.event_id, rc->reg_ntf.event_parameter);
        if (rc->reg_ntf.event_id == ESP_AVRC_RN_VOLUME_CHANGE) {
            s_volume_notify = true;
            esp_avrc_rn_param_t rn_param;
            rn_param.volume = s_volume;
            esp_avrc_tg_send_rn_rsp(ESP_AVRC_RN_VOLUME_CHANGE, ESP_AVRC_RN_RSP_INTERIM, &rn_param);
        }
        break;
    }
    case ESP_AVRC_TG_REMOTE_FEATURES_EVT: {
        ESP_LOGI(BT_AVRCP_TAG, "AVRC remote features %x, CT features %x", rc->rmt_feats.feat_mask, rc->rmt_feats.ct_feat_flag);
        break;
    }
    default:
        ESP_LOGE(BT_AVRCP_TAG, "%s unhandled evt %d", __func__, event);
        break;
    }
}
