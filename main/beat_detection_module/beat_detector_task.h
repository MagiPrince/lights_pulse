#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "esp_log.h"
#include "beat_detector.h"

typedef struct beat_detector_t{
    uint16_t* array_f;
    float complex* array_f_chapeau;
    float complex** array_wn;
    float* array_f_norm;
    uint16_t index_max_freq_value;
    float* array_average;
    uint16_t id;
    QueueHandle_t* queue_sample;
    QueueHandle_t* queue_dmx;
}beat_detector_t;

void init_beat_detector_t(QueueHandle_t* queue_sample, QueueHandle_t* queue_dmx);
void free_beat_detector_t();
void create_beat_detector_tasks();