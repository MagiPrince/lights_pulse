#include "MF078.h"
#include <time.h>

void mf078Reset(dmx_t* dmx, device_t* movingLight){
    for (int i = 0; i < movingLight->length; i++) {
        movingLight->data[i] = 0;
    }
    actualiseDevice(dmx,movingLight);
}
void mf078PanPosition(dmx_t* dmx, device_t* movingLight, char position){
    movingLight->data[PAN] = position;
    actualiseDevice(dmx,movingLight);
}
void mf078ResetPan(dmx_t* dmx, device_t* movingLight){
    movingLight->data[PAN] = 0;
    actualiseDevice(dmx,movingLight);
}
void mf078TiltPosition(dmx_t* dmx, device_t* movingLight, char position){
    movingLight->data[TILT] = position;
    actualiseDevice(dmx,movingLight);
}
void mf078ResetTilt(dmx_t* dmx, device_t* movingLight){
    movingLight->data[TILT] = 0;
    actualiseDevice(dmx,movingLight);
}
void mf078FastStrobe(dmx_t* dmx, device_t* movingLight){
    movingLight->data[LIGHT_EFFECT] = FAST_STROBE;
    actualiseDevice(dmx,movingLight);
}
void mf078SlowStrobe(dmx_t* dmx, device_t* movingLight){
    movingLight->data[LIGHT_EFFECT] = SLOW_STROBE;
    actualiseDevice(dmx,movingLight);
}
void mf078LightOn(dmx_t* dmx, device_t* movingLight){
    movingLight->data[LIGHT_EFFECT] = 255;
    actualiseDevice(dmx,movingLight);
}
void mf078LightOff(dmx_t* dmx, device_t* movingLight){
    movingLight->data[LIGHT_EFFECT] = 0;
    actualiseDevice(dmx,movingLight);
}
void mf078LowIntensity(dmx_t* dmx, device_t* movingLight){
    movingLight->data[LIGHT_EFFECT] = 9;
    actualiseDevice(dmx,movingLight);
}
void mf078RedLight(dmx_t* dmx, device_t* movingLight, char intensity){
    movingLight->data[RED] = intensity;
    actualiseDevice(dmx,movingLight);
}
void mf078GreenLight(dmx_t* dmx, device_t* movingLight, char intensity){
    movingLight->data[GREEN] = intensity;
    actualiseDevice(dmx,movingLight);
}
void mf078BlueLight(dmx_t* dmx, device_t* movingLight, char intensity){
    movingLight->data[BLUE] = intensity;
    actualiseDevice(dmx,movingLight);
}
void mf078WhiteLight(dmx_t* dmx, device_t* movingLight, char intensity){
    movingLight->data[WHITE] = intensity;
    actualiseDevice(dmx,movingLight);
}
void mf078Speed(dmx_t* dmx, device_t* movingLight, char speed){
    movingLight->data[SPEED] = speed;
    actualiseDevice(dmx,movingLight);
}
void mf078ResetLightColors(dmx_t* dmx, device_t* movingLight){
    for (int i = RED; i < WHITE+1; i++) {
        movingLight->data[i] = 0;
    }
    actualiseDevice(dmx,movingLight);
}
void mf078RandomColors(dmx_t* dmx, device_t* movingLight){
    srand(time(NULL));
    for (int i = RED; i < WHITE+1; i++) {
        movingLight->data[i] = rand() % 256;
    }
    actualiseDevice(dmx,movingLight);
}
void mf078RandomPanTilt(dmx_t* dmx, device_t* movingLight){
    srand(time(NULL));
    for (int i = PAN; i < TILT+1; i++) {
        movingLight->data[i] = rand() % 256;
    }
    actualiseDevice(dmx,movingLight);
}
