#ifndef TOUCH_SENSOR_H
#define TOUCH_SENSOR_H

#include <stdint.h>

void TouchSensor_Init(void);
uint8_t TouchSensor_Read(void);
void TouchSensor_SetCallback(void (*callback)(void), uint32_t period);

#endif // TOUCH_SENSOR_H