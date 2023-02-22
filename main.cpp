#include "Display.h"
//#include "Comms.h"
//#include "Environment.h"

#include <stdio.h>
#include <iostream>
#include <string.h>
#include <string>

#include "pico/stdlib.h"
#include "boards/pimoroni_tiny2040.h"

//#include "hardware/i2c.h"
#include "hardware/uart.h"
#include "hardware/irq.h"

// Include custom Temp/Humid library
#include "pico-sht3x/sht3x.h"

// Define pins for UART (only for intitial debugging)
#define UART_TX_PIN 16
#define UART_RX_PIN 17

#define UART_ID     uart0
#define BAUD_RATE   115200
#define DATA_BITS   8
#define STOP_BITS   1
#define PARITY      UART_PARITY_NONE

// Define pins for Stepper Motor
#define MOT_ENABLE  0
#define MOT_MS1     1
#define MOT_MS2     2
#define MOT_MS3     3
#define MOT_RST     4
#define MOT_SLP     5
#define MOT_STEP    6
#define MOT_DIR     7

//#define I2C_PORT    'i2c1'
#define I2C_SDA     26
#define I2C_SCL     27

uint32_t rxCount;
char rxBuff[16];
bool rxValid;

char splitTemp[4];

static int chars_rxed = 0;

void on_uart_rx();

float       temperature = 0.0;
float       humidity = 0.0;
uint32_t    currentPosition = 0;
uint32_t    targetPosition = 0;

using namespace std;

int main() {

    rxCount = 0;
    rxValid = false;

    // Init I/O
    stdio_init_all();




    // Init Temp UART
    uart_init( UART_ID, BAUD_RATE );

    //  Using alternate pins because I'm using the Pimoroni Tiny as efficiently as I can
    gpio_set_function( UART_TX_PIN, GPIO_FUNC_UART );
    gpio_set_function( UART_RX_PIN, GPIO_FUNC_UART );

    // Set UART flow control CTS/RTS, we don't want these, so turn them off
    uart_set_hw_flow(UART_ID, false, false);

    // Set our data format
    uart_set_format(UART_ID, DATA_BITS, STOP_BITS, PARITY);

    // Turn off FIFO's - we want to do this character by character
    uart_set_fifo_enabled(UART_ID, false);

    // Set up a RX interrupt
    // We need to set up the handler first
    // Select correct interrupt for the UART we are using
    int UART_IRQ = uart0 == uart0 ? UART0_IRQ : UART1_IRQ;

    // And set up and enable the interrupt handlers
    irq_set_exclusive_handler(UART_IRQ, on_uart_rx);
    irq_set_enabled(UART_IRQ, true);

    // Now enable the UART to send interrupts - RX only
    uart_set_irq_enables(UART_ID, true, false);

    // OK, all set up.
    // Lets send a basic string out, and then run a loop and wait for RX interrupts
    // The handler will count them, but also reflect the incoming data back with a slight change!
    uart_puts(UART_ID, "\nHello, uart interrupts\n");



    // Init I2C
    i2c_init( i2c1, 1000000);
    gpio_set_function( I2C_SDA, GPIO_FUNC_I2C );
    gpio_set_function( I2C_SCL, GPIO_FUNC_I2C );
    gpio_pull_up( I2C_SDA );
    gpio_pull_up( I2C_SCL );

    // Wait for i2c to settle down before initializing the display (otherwise it won't function correctly)
    sleep_ms( 250 );

    // Init OLED screen
    pico_ssd1306::SSD1306 oled = pico_ssd1306::SSD1306( i2c1, 0x3C, pico_ssd1306::Size::W128xH32 );
    oled.setOrientation(0);
    oled.setContrast( 64 );

    // Init display functions
    display::Display Display;

    // Init Temperature and Humidity Sensor
    pico_sht3x::SHT3X TempHumid = pico_sht3x::SHT3X( i2c1 );
    TempHumid.init_2hz();

    #ifndef PICO_DEFAULT_LED_PIN
        #warning blink example requires a board with a regular LED
    #else
        const uint G_LED_PIN = PICO_DEFAULT_LED_PIN;

        gpio_init(G_LED_PIN);
        gpio_init( 18 ); // Red
        gpio_init( 20 ); // Blue
        gpio_set_dir(18, GPIO_OUT);
        gpio_set_dir(G_LED_PIN, GPIO_OUT);
        gpio_set_dir(20, GPIO_OUT);
        gpio_put(G_LED_PIN, 1);
        gpio_put( 18, 0 );
        gpio_put( 20, 1 );

        uint8_t ledStatus = 0;

        float Temp = 0.0, Humid = 0.0;

        while (true) {

            ledStatus = 1 - ledStatus;

            gpio_put( G_LED_PIN, ledStatus );

            TempHumid.read_c( &temperature, &humidity );

            Display.setTemp( temperature );
            Display.setHumid( humidity );
            Display.setTargetPosition( targetPosition );
            Display.setCurrentPosition( currentPosition );

            Display.updateScreen( oled );


            sleep_ms(1000);
        }

    #endif
}


void on_uart_rx()
{
    while (uart_is_readable(UART_ID)) {
        uint8_t ch = uart_getc(UART_ID);

        if( ch == ':' )
        {
            chars_rxed = 0;
        }

        if( chars_rxed < 16 )
        {
            rxBuff[chars_rxed] = ch;
        }

        if( ch == '#' )
        {
            rxValid = true;
        }

        chars_rxed++;

        if( rxValid )
        {
            // Setters
            if( rxBuff[1] == 'S' )
            {
                if( rxBuff[2] == 'N' )
                {
                    targetPosition = 0;

                    // :SNxxxx,xxxx#
                    // Total characters - 4 gives length to extract. Start at the third place for the first digit.

                    targetPosition = stoul( string(rxBuff).substr(3, chars_rxed-4), nullptr, 16 );

                }
            }

            // Getters
            else if( rxBuff[1] == 'G' )
            {
                char feedback[8];

                if( rxBuff[2] == 'N' )
                {
                    // Feedback target position data
                    // :GNxxxx,xxxx#

                    //char feedback[8];

                    sprintf( feedback, "%x", targetPosition );

                    // uart_puts( UART_ID, feedback );
                    // uart_putc( UART_ID, '#' );
                }
                else if( rxBuff[2] == 'P' )
                {
                    // Feedback current position data
                    // :GPxxxx,xxxx#

                    //char feedback[8];

                    sprintf( feedback, "%x", currentPosition );

                    // uart_puts( UART_ID, feedback );
                    // uart_putc( UART_ID, '#' );
                }
                else if( rxBuff[2] == 'T' )
                {
                    // Feedback temperature data
                    // :GTxxxx,xxxx#

                    // Double the temperature, then turn it into a signed int
                    sprintf( feedback, "%08x", int16_t( temperature*2 ));


                    // uart_puts( UART_ID, feedback );
                    // uart_putc( UART_ID, '#' );
                }

                uart_puts( UART_ID, feedback );
                uart_putc( UART_ID, '#' );

            }

            chars_rxed = 0;
            rxValid = false;
        }
    }
}

void rx_interp(  )
{

}