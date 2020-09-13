#include "tdf.h"

/**
 * @brief Create a matrix W where w = e^-(2πi/N) to do a tdf
 * 
 * @return float complex** containing the matrix W
 */
float complex** wn(){
    float complex w = cexpf(-2*M_PI*I/N);

    float complex** array_wn = (float complex**) malloc(sizeof(float complex*)*N);
    for (int i = 0; i < N; i++)
    {
        array_wn[i] = (float complex*) malloc(sizeof(float complex) *N);
    }
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            array_wn[i][j] = cpowf(w, i*j);
        }
    }
    return array_wn;
}

// 
/**
 * @brief Do a fourier transform doing the multiplication of matrix W and the vector f
 * 
 * @param array_f contain all the values to be transformed
 * @param array_wn contain the matrix to do the fourier transform
 * @return float complex* Containing the result of the fourier transform
 */
float complex* tdf(float* array_f, float complex** array_wn){
    float complex* array_f_chapeau = (float complex*) malloc(sizeof(float complex) *N);
    float complex res_temp = I;
    for (int i = 0; i < N; i++)
    {
        res_temp = I;
        for (int j = 0; j < N; j++)
        {
             res_temp += array_wn[i][j]*array_f[j];
        }
        array_f_chapeau[i] = res_temp;
    }

    return array_f_chapeau;
}

/**
 * @brief Do the Fourier transform inverse by multiplicate the matrix W^-1 and the vector f_chapeau
 * 
 * @param array_f the array that will contain the result
 * @param array_f_chapeau contain complex numbers to be transformed
 * @param mat_inv contain the inversed matrix of W => W^-1
 */
void itdf(float* array_f, float complex* array_f_chapeau, float complex mat_inv[][N]){
    float res_temp = 0.0;
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            res_temp = res_temp + crealf(mat_inv[i][j]*array_f_chapeau[j]);
        }
        array_f[i] = res_temp;
        res_temp = 0.0;
    }
}

/**
 * @brief Normalise an array of int16 to an array of float in a range between -1 and 1
 * 
 * @param array_f the array to be normalised
 * @return float* array normalised
 */
float* normalisation(uint16_t* array_f){
    float* array_f_norm = malloc(sizeof(float)*128);
    int val = cpowf(2,16);
    for (int i = 0; i < N; i++)
    {
        array_f_norm[i] = (float)array_f[i]/(float)val;
    }
    return array_f_norm;
}

/**
 * @brief Denormalise an array of float between -1 and 1 to an array of int16
 * 
 * @param array_f array that will contain the data denormalised
 * @param array_f_norm array containing the data to denormalised
 */
void denormalisation(uint16_t* array_f, float* array_f_norm){
    int val = cpowf(2,16);
    for (int i = 0; i < N; i++)
    {
        array_f[i] = (uint16_t)round(array_f_norm[i]*val);
    }
    free(array_f_norm);
}