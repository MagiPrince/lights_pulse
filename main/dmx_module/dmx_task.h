#ifndef __DMX_TASK_H__
#define __DMX_TASK_H__

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "dmx.h"
#include "MF078.h"
#include "MPX405.h"
<<<<<<< HEAD

void createDmxTasks();
=======
#include <stdint.h>

void createDmxTasks(xQueueHandle* dmxQueue);
>>>>>>> modules_merge

#endif
