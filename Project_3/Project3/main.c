#include <stdio.h>
#include <stdlib.h>
#include "platform.h"
#include "timer.h"
#include "dht11.h"
#include "touch_sensor.h"
#include "ext_led.h"
#include "uart.h"
#include "gpio.h"
#include "delay.h"
#include "queue.h"

#define TEMP_THRESHOLD_HIGH 25
#define TEMP_THRESHOLD_LOW 20
#define DEFAULT_PRINT_PERIOD 2
#define BUFF_SIZE 128

Queue rx_queue;

volatile int touch_press_count = 0;
volatile int temp_print_period = DEFAULT_PRINT_PERIOD;  // Default period for temperature printing in seconds
volatile uint8_t temperature = 0;
volatile uint8_t humidity = 0;
volatile int user_id = 0;
volatile int timer_counter = 0;
volatile int read_sensor_flag = 0;
volatile int touch_event_flag = 0;
Pin ext_led_pin = PB_5; // Assuming the LED is connected to port B, pin 5
Pin touch_sensor_pin = PA_1;

// Function prototypes
void timer_callback(void);
void touch_callback(void);
void uart_rx_callback(uint8_t c);
int get_last_two_digits_sum(int id);
void handle_led_logic(void);
void TouchSensor_Init(void);

int main(void) {
    uint8_t rx_char = 0;
    char buff[BUFF_SIZE];
    uint32_t buff_index;

    // Initialize peripherals and UART
    queue_init(&rx_queue, 128);  // Initialize receive queue
    uart_init(115200);           // Initialize UART communication
    uart_set_rx_callback(uart_rx_callback);
    uart_enable();

    gpio_set_mode(ext_led_pin, Output);

    // Request user ID through UART interface
    uart_print("Enter an AEM code: ");
    buff_index = 0;

    // Receive characters until Enter is pressed or buffer is full
    do {
        while (!queue_dequeue(&rx_queue, &rx_char)) {
            __WFI();
        }
        if (rx_char == 0x7F) { // Handle backspace character
            if (buff_index > 0) {
                buff_index--;
                uart_tx(rx_char);
            }
        } else {
            // Store and echo the received character
            buff[buff_index++] = (char)rx_char;
            uart_tx(rx_char);
        }
    } while (rx_char != '\r' && buff_index < BUFF_SIZE);

    // Replace the last character with \0
    buff[buff_index - 1] = '\0';
    uart_print("\r\n");

    // Check if AEM code is valid
    sscanf(buff, "%d", &user_id);
    uart_print("User's AEM received.\r\n");
    /*char str[BUFF_SIZE];
		sprintf(str,"User's AEM received.\r\n", user_id);
		uart_print(str);
		*/

    // Initialize peripherals
    DHT11_Init();
    TouchSensor_Init();
    gpio_set_callback(PA_1, touch_callback);
    gpio_set(ext_led_pin, LED_OFF);

    // Initialize timer 
    timer_init(1000000);  // 1-second period
    timer_set_callback(timer_callback);
    timer_enable();

    __enable_irq(); // Enable interrupts

    while (1) {
        if (read_sensor_flag) {
            read_sensor_flag = 0;
            char buffer[50];
            if (DHT11_ReadTemperature((uint8_t*)&temperature, (uint8_t*)&humidity)) {
                snprintf(buffer, sizeof(buffer), "Temperature: %d C\r\n", temperature);
                uart_print(buffer);
						         			
  							// If touch count is even, also read and print humidity
                 if (touch_press_count > 1 && touch_press_count % 2 == 0) {
                     snprintf(buffer, sizeof(buffer), "Humidity: %d %%\r\n", humidity);
                     uart_print(buffer);
                 }
								snprintf(buffer, sizeof(buffer), "Sample Rate: %d seconds\r\n", temp_print_period);
                uart_print(buffer);
								timer_enable();
            } else {
                uart_print("Failed to read from DHT11 sensor\r\n");
            }
        }

        if (touch_event_flag) {
            touch_event_flag = 0;
            char buffer[50];
            snprintf(buffer, sizeof(buffer), "Touch sensor activated! Count: %d\r\n", touch_press_count);
            uart_print(buffer);

            if (touch_press_count == 1) {
                // Change temperature print period based on last 2 elements of user ID
                int last_two_digits_sum = get_last_two_digits_sum(user_id);
                if (last_two_digits_sum == 2) {
                    temp_print_period = 4;
                } else if (last_two_digits_sum == 0) {
                    temp_print_period = DEFAULT_PRINT_PERIOD;
                } else {
                    temp_print_period = last_two_digits_sum;
                }
            } else {
                if (touch_press_count % 2 == 1) {
                    // Odd press count, set period to 3 sec without printing humidity
                    temp_print_period = 3;
                }
            }

            // Reset the counter for new period
            timer_counter = 0;
					//	timer_enable();
        }
    }
}

// UART RX Callback
void uart_rx_callback(uint8_t rx) {
    if ((rx >= '0' && rx <= '9') || rx == 0x7F || rx == '\r') {
        // Store the received character
        queue_enqueue(&rx_queue, rx);
    }
}

// Timer callback to update counter and set flag for sensor reading
void timer_callback(void) {
		 timer_counter++;
	   handle_led_logic();
    if (timer_counter == temp_print_period) {
        timer_counter = 0;
        read_sensor_flag = 1;
			 // timer_disable();
    }
}

// Touch sensor init & callback
void TouchSensor_Init(void) {
    gpio_set_mode(touch_sensor_pin, Input);
	  gpio_set_mode(touch_sensor_pin, PullDown);
   	gpio_set_trigger(touch_sensor_pin, Rising);
}

void touch_callback(void) {
	  //timer_disable();
  	touch_press_count++;
    touch_event_flag = 1;
}

int get_last_two_digits_sum(int id) {
    int last_two_digits = id % 100;
    int digit1 = last_two_digits / 10;
    int digit2 = last_two_digits % 10;
    return digit1 + digit2;
}

void handle_led_logic(void) {
    if (temperature > TEMP_THRESHOLD_HIGH) {
        gpio_set(ext_led_pin, 1); // Turn on the LED
    } else if (temperature < TEMP_THRESHOLD_LOW) {
        gpio_set(ext_led_pin, 0); // Turn off the LED
    } else {
        static int led_state = 0;
        led_state = !led_state;
        gpio_set(ext_led_pin, led_state); // Toggle LED
    }
}
