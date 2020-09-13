#include "dmx.h"

void initDevice(dmx_t* dmx,device_t* device, char length,char adresse){
    device->length = length;
    device->adresse = adresse;
    for(int i=0;i<device->length;i++){
        device->data[i] = 0;
        dmx->dmxArray[device->adresse + i] = device->data[i];
    }
    dmx->nbChannelsUsed += device->length;
}
void initDmx(dmx_t* dmx){
    dmx->nbChannelsUsed = 0;
    for (int i = 0; i < 513; i++) {
        dmx->dmxArray[i] = 0;
    }
    dmx->dirty = 0;
}
void actualiseDevice(dmx_t* dmx, device_t* device){
    for(int i=0;i < device->length;i++){
        dmx->dmxArray[device->adresse + i] = device->data[i];
    }
    dmx->dirty = 1;
}

void uart_init() {
    const uart_config_t uart_config = {
        .baud_rate = 90909, // then change it to 250000 (250kbits/s)
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_2,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM_1, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
}
void send_dmx(dmx_t* dmx){
  // send start
  uart_set_baudrate(UART_NUM_1,90909);
  uart_write_bytes(UART_NUM_1,"\0",1);
  for(int i=0;i<2500;i++){
    continue;
  }
  uart_set_baudrate(UART_NUM_1,250000);
  char starting = 0;
  uart_write_bytes(UART_NUM_1,(const char*)&starting,1);
  uart_write_bytes(UART_NUM_1,(const char*)&dmx->dmxArray,dmx->nbChannelsUsed);
  for(int i=0;i<2500;i++){
    continue;
  }
}
