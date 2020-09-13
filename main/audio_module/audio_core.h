/*
 * @audio_core.h
 * @brief Audio module core library
 *
 * @Created on: September 3, 2019
 * @Author:     Stephane Oliveira, stephane.oliveira-paredes@etu.hesge.ch
 * @Bug:        No known bugs
 * @Date:       September 4, 2019
 * @Version:    1.1
 */
#ifndef __AUDIO_CORE_H__
#define __AUDIO_CORE_H__
#include <stdbool.h>

#include "driver/spi_master.h"


typedef struct {
	uint16_t*	samples;
	uint8_t		sample_len;
	bool		samplesReady;
} audio_task_params_t;

void vTask_Audio_In(void *pvParameters);


void audio_in_init(TaskHandle_t *audioHandle, audio_task_params_t *params);
void audio_sample_start(void);
void audio_sample_stop(void);

#endif