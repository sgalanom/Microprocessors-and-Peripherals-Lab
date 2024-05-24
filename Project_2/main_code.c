#include <stdio.h>
#include <stdint.h>
#include "platform.h"
#include "uart.h"
#include "queue.h"
#include "leds.h"
#include "gpio.h"
#include "timer.h"

#define BUFF_SIZE 128

Queue rx_queue;

volatile int switch_press_count = 0;
volatile uint8_t led_state = LED_OFF;

// ISR for UART receive
void uart_rx_isr(uint8_t rx) {
    if ((rx >= '0' && rx <= '9') || rx == 0x7F || rx == '\r') {
        // Store the received character
        queue_enqueue(&rx_queue, rx);
    }
}

// Function to initialize the switch
void switch_init(void) {
	gpio_set_mode(P_SW, PullDown);
  // Set up on-board switch.
	gpio_set_trigger(P_SW, Rising);
}

// Function to check the state of the switch
int switch_get(Pin pin) {
	return !gpio_get(pin);
}

// ISR for switch press event
void button_press_isr(void) {
		switch_press_count++;		// Increment switch press count
					
    // Toggle LED if it's off, turn off if it's on and print its state
		if (led_state == LED_OFF) {
				led_state = LED_ON;
				uart_print("\nLED state: ON\r\n");
				gpio_set(P_LED_R, LED_ON);
		 } else {
			  led_state = LED_OFF;
				uart_print("\nLED state: OFF\r\n");
				gpio_set(P_LED_R, LED_OFF);
		 }
		 
     // Print switch press count
		 char str[BUFF_SIZE];
		 sprintf(str, "Switch pressed %d times.\r\n", switch_press_count);
		 uart_print(str);		 		 
		 timer_disable();		// Disable the timer
}

// ISR for odd digit received
void odd_digit_isr() {
    // Toggle LED every 0.5 seconds
    led_state = (led_state == LED_OFF) ? LED_ON : LED_OFF;
    gpio_set(P_LED_R, led_state);
	  timer_enable();		// Enable timer
}

// ISR for even digit received
void even_digit_isr() {
  // Disable timer to maintain the LED state
	timer_disable();
	
}

// Timer callback function
void timer_callback() {
		odd_digit_isr();
}

int main() {
    uint8_t rx_char = 0;
    char buff[BUFF_SIZE];
    uint32_t buff_index;
    int number; 
		char led[BUFF_SIZE];
		
    
    queue_init(&rx_queue, 128);     // Initialize receive queue
    uart_init(115200);    // Initialize UART communication
    uart_set_rx_callback(uart_rx_isr);
    uart_enable();
    leds_init();    // Initialize LEDs
    switch_init();    // Initialize the switch
    gpio_set_callback(P_SW, button_press_isr); // Set callback for switch press
		timer_init(500000);
	  //timer_init(CLK_FREQ/16.8);
    timer_set_callback(timer_callback); // Set the timer callback function
		timer_disable();    // Disable timer
   
    __enable_irq();    // Enable interrupts
    
    uart_print("\r");
    while(1) {
        // Handle switch input priority
        if (switch_get(P_SW)) {
          button_press_isr(); // Call button press ISR
		  	} else {
					  // Prompt user to enter an AEM code
            uart_print("Enter an AEM code: ");
            buff_index = 0;
						// Receive characters until Enter is pressed or buffer is full
            do {
                while (!queue_dequeue(&rx_queue, &rx_char))
                    __WFI();
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
            if (buff_index < BUFF_SIZE) {
                // Convert buffer contents to integer
                if (sscanf(buff, "%d", &number) == 1) {
                    // Check if the last digit is odd or even
                    int last_digit = number % 10;
                    if (last_digit % 2 == 0) {
                        even_digit_isr(); // Call even-digit ISR
												// Print LED state
												if (led_state == LED_ON)
													uart_print("\nLED state: ON\r\n");
												else
													uart_print("\nLED state: OFF\r\n");
                    } else {
                        odd_digit_isr(); // Call odd-digit ISR	
                        // Print LED state
												if (led_state == LED_ON)
													uart_print("\nLED state: ON\r\n");
												else
													uart_print("\nLED state: OFF\r\n");	
											  uart_print("\nThe LED is blinking\r\n");
                    }
                } else {
										uart_print(buff);
                    uart_print(" invalid!\r\n");
                }
            } else {
                uart_print("Please enter AEM!\r\n");
            }
        }
    }
}
