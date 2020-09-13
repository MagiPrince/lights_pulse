#include <stdlib.h>
#include <stdio.h>
#include <tgmath.h>
#include <complex.h>
#include <time.h>
#include <stdint.h>

#define N 64
#define fs 44100

float complex** wn();
void tdf(float* array_f, float complex** array_wn, float complex* array_f_chapeau);
void itdf(float* array_f, float complex* array_f_chapeau, float complex mat_inv[][N]);
float* normalisation(uint16_t* array_f, float* array_f_norm);
void denormalisation(uint16_t* array_f, float* array_f_norm);