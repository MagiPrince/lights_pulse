#ifndef __DMX_TASK_H__
#define __DMX_TASK_H__

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "../dmx_module/dmx.h"
#include "../dmx_module/MF078.h"
#include "../dmx_module/MPX405.h"

void createDmxTasks();

#endif
