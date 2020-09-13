#include <complex.h>
#include <stdio.h>
#include <stdlib.h>
#include <tgmath.h>
#include <stdint.h>
#include "tdf.h"
#define H 345
#define N 64
#define fs 44100

uint16_t index_max_freq(uint16_t max_freq);
float energy(float complex* array_f_chapeau, uint16_t index_max_freq_value);
float average(float array_average[], float energy_value, uint16_t* id);
float variance(float average_value, float array_average[]);
float threshold(float variance_value, float average_value);
uint8_t beat_detector(uint16_t* array_f, float complex* array_f_chapeau, float* array_f_norm, float complex** array_wn, uint16_t index_max_freq_value, float* array_average, uint16_t* id);