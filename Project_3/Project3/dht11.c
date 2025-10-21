#include "dht11.h"
#include "gpio.h"
#include "delay.h"

#define DHT11_PIN PA_0  // GPIO pin number connected to DHT11

Pin dht11_pin = DHT11_PIN;

void DHT11_Start(void) {
    gpio_set_mode(dht11_pin, Output);
    gpio_set(dht11_pin, 0);
    delay_ms(18);
    gpio_set(dht11_pin, 1);
    delay_us(20);
    gpio_set_mode(dht11_pin, Input);
}

uint8_t DHT11_ReadByte(void) {
    uint8_t byte = 0;
    for (int i = 0; i < 8; i++) {
        while (!gpio_get(dht11_pin));
        delay_us(40);
        if (gpio_get(dht11_pin)) {
            byte |= (1 << (7 - i));
        }
        while (gpio_get(dht11_pin));
    }
    return byte;
}

uint8_t DHT11_ReadTemperature(uint8_t* temperature, uint8_t *humidity) {
    uint8_t data[5] = {0};
    DHT11_Start();
     
   		while (gpio_get(dht11_pin));
	    while (!gpio_get(dht11_pin));
      while (gpio_get(dht11_pin));

    for (int i = 0; i < 5; i++) {
        data[i] = DHT11_ReadByte();
    }

    if (data[4] == (data[0] + data[1] + data[2] + data[3])) {
        *temperature = data[2];
				*humidity = data[0];
        return 1;
    }
    return 0;
}

void DHT11_Init(void) {
    gpio_set_mode(dht11_pin, Input);
		gpio_set_mode(dht11_pin, PullUp);
    delay_ms(1000);  // Initial delay
}
