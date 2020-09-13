#include "beat_detector_task.h"

beat_detector_t* beat_detector_struct = NULL;

void init_beat_detector_t(QueueHandle_t* queue_sample, QueueHandle_t* queue_dmx){
    beat_detector_struct = (beat_detector_t*)malloc(sizeof(beat_detector_t));
    beat_detector_struct->array_average = malloc(sizeof(float)*H);
    beat_detector_struct->array_f = malloc(sizeof(uint16_t)*N);
    beat_detector_struct->array_f_chapeau = (float complex*) malloc(sizeof(float complex) *N);
    beat_detector_struct->array_wn = wn();
    beat_detector_struct->array_f_norm = malloc(sizeof(float)*128);
    beat_detector_struct->id = 0;
    beat_detector_struct->index_max_freq_value = index_max_freq(1000);
    beat_detector_struct->queue_sample = queue_sample;
    beat_detector_struct->queue_dmx = queue_dmx;
}

void free_beat_detector_t(){
    free(beat_detector_struct->array_average);
    free(beat_detector_struct->array_f);
    for (int i = 0; i < N; i++)
    {
        free(beat_detector_struct->array_wn[i]);
    }
    free(beat_detector_struct->array_wn);
    free(beat_detector_struct->array_f_chapeau);
    free(beat_detector_struct->array_f_norm);
}

static void beat_detector_task(void* arg){
    beat_detector_t* b_d = (beat_detector_t*)arg;
    float* array_average = b_d->array_average;
    float complex** array_wn = b_d->array_wn;
    uint16_t index_max_freq_value = b_d->index_max_freq_value;
    uint16_t id = b_d->id;

    for(;;){
        if (pdTRUE == xQueueReceive(*(b_d->queue_sample), b_d->array_f, portMAX_DELAY)){
            uint8_t beat = beat_detector(b_d->array_f, b_d->array_f_chapeau, b_d->array_f_norm, array_wn, index_max_freq_value, array_average, &id);
            if(beat){
                xQueueSend(*(b_d->queue_dmx), &beat, NULL);
            }
        }
    }
}

void create_beat_detector_tasks(void){
    
    ESP_LOGI("BEAT_DETECTOR_TAG", "Beat Detector initialized sucessfully");
    xTaskCreate(beat_detector_task, "beat_detector_task", 1024*2, beat_detector_struct, configMAX_PRIORITIES-2, NULL);
}