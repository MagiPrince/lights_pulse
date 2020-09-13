#include "MPX405.h"

void mpx405AllOff(dmx_t* dmx, device_t* lights){
    for (int i = 0; i < lights->length; i++) {
        lights->data[i] = 0;
    }
    actualiseDevice(dmx,lights);
}
void mpx405OneOff(dmx_t* dmx, device_t* lights, int light_Num){
    lights->data[light_Num] = 0;
    actualiseDevice(dmx,lights);
}
void mpx405OneOn(dmx_t* dmx, device_t* lights, int light_Num){
    lights->data[light_Num] = 255;
    actualiseDevice(dmx,lights);
}
void mpx405AllOn(dmx_t* dmx, device_t* lights){
    for (int i = 0; i < lights->length; i++) {
        lights->data[i] = 255;
    }
    actualiseDevice(dmx,lights);
}
void mpx405SetIntensity(dmx_t* dmx, device_t* lights, int light_Num, char intensity){
    lights->data[light_Num] = intensity;
    actualiseDevice(dmx,lights);
}
void mpx405AllSetIntensity(dmx_t* dmx, device_t* lights, char intensity){
    for (int i = 0; i < lights->length; i++) {
        lights->data[i] = intensity;
    }
    actualiseDevice(dmx,lights);
}
void mpx405ToggleLights(dmx_t* dmx, device_t* lights){
    for (int i = 0; i < lights->length; i++) {
        lights->data[i] = ~lights->data[i];
    }
    actualiseDevice(dmx,lights);
}

void mpx405ToggleLight(dmx_t* dmx, device_t* lights, int light_Num){
    lights->data[light_Num] = ~lights->data[light_Num];
    actualiseDevice(dmx,lights);
}
