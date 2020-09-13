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
    //printf("%lf%+lfi\n", creal(variance_value), cimag(variance_value));
    return (variance_value + average_value)*1.065;
    //return 1.08;//1.398;
    //return -0.0025714*variance_value+1.3;
}

/**
 * @brief Detecte if there is a beat with the actual energy compared to the threshold
 * 
 * @param energy_value the value of the energy
 * @param average_value the value of the average of energies
 * @param threshold_value the value of the threshold
 * @return uint8_t "1" or "0" if there is a beat or not
 */
uint8_t beat_detector(float energy_value, float threshold_value){

    //printf("E : %f, T : %f, A : %f\n",(energy_value), (threshold_value), average_value);
    if(energy_value > (threshold_value)){
        return 1;
    }
    return 0;
}