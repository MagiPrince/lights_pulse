#include "beat_detector.h"


/**
 * @brief Check the last index to be smaller than the max_freq
 * 
 * @param max_freq correspond to the max frequency we want to find in the array
 * @return the index of an array of size "N" before depassing the indicated frequency
 */
uint16_t index_max_freq(uint16_t max_freq){
    for(int i = 0; i < N; i++){
        if((i * fs/N) > max_freq){
            return i-1;
        }
    }
    return N;
}

/**
 * @brief Calculate the energy of the indicated frequencies from a sample in complex numbers
 * 
 * @param array_f_chapeau sample where to take the values
 * @param index_max_freq_value correspond to the index max containing the values desired
 * @return float of the modulus of the energy calculated
 */
float energy(float complex* array_f_chapeau, uint16_t index_max_freq_value){

    float tmp = 0.0;

    for (int i = 0; i < index_max_freq_value; i++)
    {
        tmp += (cpowf(crealf(array_f_chapeau[i])*crealf(array_f_chapeau[i]),2)+cpowf(cimagf(array_f_chapeau[i])*cimagf(array_f_chapeau[i]),2));
    }
    
    return tmp/index_max_freq_value;
}

/**
 * @brief Calculate the average of the last second of energies
 * 
 * @param array_average contain all energies of the last second
 * @param energy_value is the actual energy value
 * @param id contain the number of data in the array array_average
 * @return float the average of the array
 */
float average(float array_average[], float energy_value, uint16_t* id){
    float tmp = 0;
    if(*id > (H-1)){
        for (int i = 0; i < (H-1); i++)
        {
            array_average[i] = array_average[i+1];
            tmp += array_average[i+1];
        }
        array_average[(H-1)] = energy_value;
        return tmp/(float)*id;
    }
    else{
        for (int i = 0; i < (H-1); i++)
        {
            array_average[i] = array_average[i+1];
            tmp += array_average[i+1];
        }
        array_average[(H-1)] = energy_value;
        *id+=1;
        return (tmp / (float)(*id-1));
    }
}

/**
 * @brief Calculate the variance of the energies
 * 
 * @param average_value the actual average
 * @param array_average contain all energies of the last second
 * @return float the variance
 */
float variance(float average_value, float array_average[]){

    float tmp = 0.0;
    for (int i = 0; i < H-1; i++)
    {
        tmp += cpowf((array_average[i] - average_value),2);
    }
    return tmp/(float)H;
}

/**
 * @brief calculate the threshold
 * 
 * @param variance_value the value of the variance of energies
 * @param average_value the value of the average of energies
 * @return float the threshold
 */
float threshold(float variance_value, float average_value){
    return (variance_value + average_value)*1.065;
}

/**
 * @brief Detecte if there is a beat with the actual energy compared to the threshold
 * 
 * @param array_f array with the values to analyse
 * @param array_f_chapeau array with Fourier transform values
 * @param array_wn matrix for the fourier transform
 * @param index_max_freq_value index max of the frequencies in the array array_f_chapeau
 * @param array_average contain the values of the average of the energie calculated of the last second
 * @param id identify where we are in the array array_average
 * @return uint8_t return "1" if he detect a beat and "0" if not
 */
uint8_t beat_detector(uint16_t* array_f, float complex* array_f_chapeau, float* array_f_norm, float complex** array_wn, uint16_t index_max_freq_value, float* array_average, uint16_t* id){

    float energy_value = 0.0;
    float average_value = 0.0;
    float variance_value = 0.0;

    normalisation(array_f, array_f_norm);
    tdf(array_f_norm, array_wn, array_f_chapeau);

    energy_value = energy(array_f_chapeau, index_max_freq_value);
    average_value = average(array_average, energy_value, id);
    //variance_value = variance(average_value, array_average);
    //printf("E : %f, A : %f\n", energy_value, average_value);
    uint8_t tmp = energy_value > (average_value*1.9);
    
    return tmp;
}