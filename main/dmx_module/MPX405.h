#ifndef __MPX405_H__
#define __MPX405_H__

#include "dmx.h"

#define LIGHT1 0
#define LIGHT2 1
#define LIGHT3 2
#define LIGHT4 3

/*
    Turn all the mpx405 lights off
        dmx : the information to send with the dmx protocol
        lights : the device to be affected by the change
*/
void mpx405AllOff(dmx_t* dmx, device_t* lights);
/*
    Turn one of the mpx405 lights off
        dmx : the information to send with the dmx protocol
        lights : the device to be affected by the change
        light_Num : the number of the light being affected
*/
void mpx405OneOff(dmx_t* dmx, device_t* lights, int light_Num);
/*
    Turn one of the mpx405 lights on
        dmx : the information to send with the dmx protocol
        lights : the device to be affected by the change
        light_Num : the number of the light being affected
*/
void mpx405OneOn(dmx_t* dmx, device_t* lights, int light_Num);
/*
    Turn all the mpx405 lights on
        dmx : the information to send with the dmx protocol
        lights : the device to be affected by the change
*/
void mpx405AllOn(dmx_t* dmx, device_t* lights);
/*
    Set an intensity for one of the mpx405 lights
        dmx : the information to send with the dmx protocol
        lights : the device to be affected by the change
        light_Num : the number of the light being affected
        intensity : the intensity of the selected light
*/
void mpx405SetIntensity(dmx_t* dmx, device_t* lights, int light_Num, char intensity);
/*
    Set an intensity for all of the mpx405 lights
        dmx : the information to send with the dmx protocol
        lights : the device to be affected by the change
        intensity : the intensity for all the lights
*/
void mpx405AllSetIntensity(dmx_t* dmx, device_t* lights, char intensity);
/*
    Toggle the intensity of all the lights
        dmx : the information to send with the dmx protocol
        lights : the device to be affected by the change
*/
void mpx405ToggleLights(dmx_t* dmx, device_t* lights);
/*
    Toggle the intensity of a single light
        dmx : the information to send with the dmx protocol
        lights : the device to be affected by the change
        light_Num : the light to toggle the intensity
*/
void mpx405ToggleLight(dmx_t* dmx, device_t* lights, int light_Num);

#endif
