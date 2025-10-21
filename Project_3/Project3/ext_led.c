#include "ext_led.h"
#include "gpio.h"

#define LED_PIN  PB_3 // GPIO pin number connected to the LED

Pin led_pin = LED_PIN;

void LED_Init(void) {
    gpio_set_mode(LED_PIN, Output);
}

void LED_On(void) {
    gpio_set(LED_PIN, 1);
}

void LED_Off(void) {
    gpio_set(LED_PIN, 0);
}
