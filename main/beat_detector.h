#include <complex.h>
#include <stdio.h>
#include <stdlib.h>
#include <tgmath.h>
#include <stdint.h>
#define H 345
#define N 128
#define fs 44100

uint16_t index_max_freq(uint16_t max_freq);
float energy(float complex* array_f_chapeau, uint16_t index_max_freq_value);
float average(float array_average[], float energy_value, uint16_t* id);
float variance(float average_value, float array_average[]);
float threshold(float variance_value, float average_value);
uint8_t beat_detector(float energy_value, float threshold_value);