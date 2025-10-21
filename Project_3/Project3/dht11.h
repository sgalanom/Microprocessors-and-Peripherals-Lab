#ifndef DHT11_H
#define DHT11_H

#include <stdint.h>

void DHT11_Init(void);
uint8_t DHT11_ReadTemperature(uint8_t* temperature, uint8_t *humidity);
void DHT11_Start(void);
uint8_t DHT11_ReadByte(void);

#endif // DHT11_H
