#ifndef __DMX_H__
#define __DMX_H__

#include "driver/uart.h"
#include "soc/uart_struct.h"


static const int RX_BUF_SIZE = 1024;

#define TXD_PIN (GPIO_NUM_17)
#define RXD_PIN (UART_PIN_NO_CHANGE)
#define GPIO_START_PIN (GPIO_NUM_4)
#define GPIO_START_PIN_SEL (1ULL<<GPIO_START_PIN)

#define BEAT_MODE 0
#define OFF_MODE 1
#define STROBE_MODE 2

#define MAX_DMX_CHANEL 512
#define MAX_DEVICE_CHANEL 20

typedef struct dmx_t{
    char nbChannelsUsed;
    char dmxArray[MAX_DMX_CHANEL];
    char dirty;
}dmx_t;

typedef struct device_t{
    char length;
    char adresse;
    char data[MAX_DEVICE_CHANEL];
}device_t;

enum dmxQueue_t{beat, off, strobe};
/*
    Initialization of a device for the dmx
        dmx : the information to send with the dmx protocol
        device : the type of device to add
        length : the length of the device's data
        adresse : the adresse of the device
*/
void initDevice(dmx_t* dmx, device_t* device, char length,char adresse);
/*
    Initialization of the dmx
        dmx : the information to send with the dmx protocol
*/
void initDmx(dmx_t* dmx);
/*
    Initialization of a device for the dmx
        dmx : the information to send with the dmx protocol
        device : the device that is actualised in the dmx array
*/
void actualiseDevice(dmx_t* dmx, device_t* device);
/*
    Initialization of a uart
*/
void uart_init();
/*
    Sending the dmx array with the dmx protocol to the external devices
        dmx : the information to send with the dmx protocol
*/
void send_dmx(dmx_t* dmx);

#endif
