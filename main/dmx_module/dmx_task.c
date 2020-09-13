#include "dmx_task.h"

<<<<<<< HEAD
static xQueueHandle dmxQueue = NULL;
=======
>>>>>>> modules_merge
static const char *DMX_TASK_TAG = "DMX TASK ";
int cnt = 0;

void strobeMode(dmx_t* dmx, device_t* mf078, device_t* mpx405){
    mpx405ToggleLights(dmx,mpx405);
    mf078RandomPanTilt(dmx, mf078);
    mf078RandomColors(dmx,mf078);
    mf078FastStrobe(dmx,mf078);
}
void offMode(dmx_t* dmx, device_t* mf078, device_t* mpx405){
    mf078Reset(dmx,mf078);
    mpx405AllOff(dmx,mpx405);
}
void beatMode(dmx_t* dmx, device_t* mf078, device_t* mpx405){
    mf078RandomPanTilt(dmx, mf078);
    mf078LightOn( dmx, mf078);
    mf078RandomColors(dmx,mf078);
    mpx405OneOn(dmx,mpx405,LIGHT1);
    if(cnt){mpx405OneOn(dmx,mpx405,LIGHT2); cnt=0;}else{cnt++;}
    vTaskDelay(50 / portTICK_PERIOD_MS);
    mf078LowIntensity(dmx,mf078);
    mpx405AllOff(dmx,mpx405);
}
static void dmx_task(void * arg)
{
    esp_log_level_set(DMX_TASK_TAG, ESP_LOG_INFO);
    dmx_t* dmx = (dmx_t*)arg;
    device_t* mpx405 = (device_t*)malloc(sizeof(device_t));
    initDevice(dmx,mpx405,4,8);
    ESP_LOGI(DMX_TASK_TAG, "First device initialized");
    device_t* mf078 = (device_t*)malloc(sizeof(device_t));
    initDevice(dmx,mf078,8,0);
    ESP_LOGI(DMX_TASK_TAG, "Second device initialized");
    ESP_LOGI(DMX_TASK_TAG, "Dmx ready");
    uint8_t valQueue;
    while (1) {
        if(xQueueReceive( dmx->dmxQueue, &valQueue, portMAX_DELAY)){
            switch (valQueue) {
                case OFF_MODE:
                    offMode(dmx, mf078, mpx405);
                break;
                case BEAT_MODE:
                    beatMode(dmx, mf078, mpx405);
                break;
                case STROBE_MODE:
                    strobeMode(dmx, mf078, mpx405);
                break;
            }
        }
    }
    free(mpx405);
    free(mf078);
    free(dmx);
}
static void dmx_send_task(void* arg){
    dmx_t* dmx = (dmx_t*) arg;
    while(1){
        if(dmx->dirty){
            send_dmx(dmx);
            dmx->dirty = 0;
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

/*static void send_test(){
    enum dmxQueue_t valQueue = 0;
    while(1){
        xQueueSendFromISR(dmxQueue, &valQueue, NULL);
        vTaskDelay(800 / portTICK_PERIOD_MS);
    }
}*/

void createDmxTasks(xQueueHandle* dmxQueue){
    uart_init();
    dmx_t* dmx = (dmx_t*)malloc(sizeof(dmx_t));
    initDmx(dmx);
    dmx->dmxQueue = dmxQueue;
    xTaskCreate(dmx_task, "dmx_task", 2048, dmx, configMAX_PRIORITIES-3, NULL);
    xTaskCreate(dmx_send_task, "dmx_send_task", 1024, dmx, configMAX_PRIORITIES-3, NULL);
    //xTaskCreate(send_test, "uart_tx_task", 1024*2, NULL, configMAX_PRIORITIES-1, NULL);
}
