#ifndef __MF078_H__
#define __MF078_H__

#include "dmx.h"

#define PAN 0
#define TILT 1
#define LIGHT_EFFECT 2
#define RED 3
#define GREEN 4
#define BLUE 5
#define WHITE 6
#define SPEED 7
#define FAST_STROBE 239
#define SLOW_STROBE 135
#define LIGHT_ON 255
#define LIGHT_OFF 0

/*
    Reset all the elements of the moving led head
        dmx : the information to send with the dmx protocol
        movingLight : the device to be affected by the change
*/
void mf078Reset(dmx_t* dmx, device_t* movingLight);

////////////////////////////////////////////////////////////////////////////////
////////////////////        PAN & TILT SETTINGS          ///////////////////////
////////////////////////////////////////////////////////////////////////////////
/*
    Move the pan to a certain position
        dmx : the information to send with the dmx protocol
        movingLight : the device to be affected by the change
        position : the position that the pan should obtain
*/
void mf078PanPosition(dmx_t* dmx, device_t* movingLight, char position);
/*
    Reset the pan initial position
        dmx : the information to send with the dmx protocol
        movingLight : the device to be affected by the change
*/
void mf078ResetPan(dmx_t* dmx, device_t* movingLight);
/*
    Move the tilt to a certain position
        dmx : the information to send with the dmx protocol
        movingLight : the device to be affected by the change
        position : the position that the tilt should obtain
*/
void mf078TiltPosition(dmx_t* dmx, device_t* movingLight, char position);
/*
    Reset the tilt initial position
        dmx : the information to send with the dmx protocol
        movingLight : the device to be affected by the change
*/
void mf078ResetTilt(dmx_t* dmx, device_t* movingLight);
/*
    Set a random position for the pan and the tilt
        dmx : the information to send with the dmx protocol
        movingLight : the device to be affected by the change
*/
void mf078RandomPanTilt(dmx_t* dmx, device_t* movingLight);
/*
    Set the speed of the pan and the tilt
        dmx : the information to send with the dmx protocol
        movingLight : the device to be affected by the change
        speed : the speed to apply to the pan and the tilt
*/
void mf078Speed(dmx_t* dmx, device_t* movingLight, char speed);




////////////////////////////////////////////////////////////////////////////////
//////////////////        LIGHT EFFECT SETTINGS          ///////////////////////
////////////////////////////////////////////////////////////////////////////////
/*
    Give a fast strobe effect to the moving head device
        dmx : the information to send with the dmx protocol
        movingLight : the device to be affected by the change
*/
void mf078FastStrobe(dmx_t* dmx, device_t* movingLight);
/*
    Give a slow strobe effect to the moving head device
        dmx : the information to send with the dmx protocol
        movingLight : the device to be affected by the change
*/
void mf078SlowStrobe(dmx_t* dmx, device_t* movingLight);
/*
    The light intensity of the moving head device is set to maximum
        dmx : the information to send with the dmx protocol
        movingLight : the device to be affected by the change
*/
void mf078LightOn(dmx_t* dmx, device_t* movingLight);
/*
    The light of the moving head device is off
        dmx : the information to send with the dmx protocol
        movingLight : the device to be affected by the change
*/
void mf078LightOff(dmx_t* dmx, device_t* movingLight);
/*
    The light intensity of the moving head device is set to minimum
        dmx : the information to send with the dmx protocol
        movingLight : the device to be affected by the change
*/
void mf078LowIntensity(dmx_t* dmx, device_t* movingLight);




////////////////////////////////////////////////////////////////////////////////
//////////////////        LIGHT COLOR SETTINGS          ///////////////////////
////////////////////////////////////////////////////////////////////////////////
/*
    Give a red intensity to the light
        dmx : the information to send with the dmx protocol
        movingLight : the device to be affected by the change
        intensity : the intensity of the red light
*/
void mf078RedLight(dmx_t* dmx, device_t* movingLight, char intensity);
/*
    Give a green intensity to the light
        dmx : the information to send with the dmx protocol
        movingLight : the device to be affected by the change
        intensity : the intensity of the green light
*/
void mf078GreenLight(dmx_t* dmx, device_t* movingLight, char intensity);
/*
    Give a blue intensity to the light
        dmx : the information to send with the dmx protocol
        movingLight : the device to be affected by the change
        intensity : the intensity of the blue light
*/
void mf078BlueLight(dmx_t* dmx, device_t* movingLight, char intensity);
/*
    Give a white intensity to the light
        dmx : the information to send with the dmx protocol
        movingLight : the device to be affected by the change
        intensity : the intensity of the white light
*/
void mf078WhiteLight(dmx_t* dmx, device_t* movingLight, char intensity);
/*
    Disable all the colors
        dmx : the information to send with the dmx protocol
        movingLight : the device to be affected by the change
*/
void mf078ResetLightColors(dmx_t* dmx, device_t* movingLight);
/*
    Put completely random colors to the moving led head
        dmx : the information to send with the dmx protocol
        movingLight : the device to be affected by the change
*/
void mf078RandomColors(dmx_t* dmx, device_t* movingLight);

#endif
