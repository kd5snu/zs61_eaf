#include "Focuser.h"
#include "Comms.h"
#include "Environment.h"

#include <stdio.h>

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

#define UART_BAUD_RATE 9600

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
char rxBuff[20];
bool rxValid;

void on_uart_rx();

int main() {

    rxCount = 0;
    rxValid = false;

    // Init I/O
    stdio_init_all();

    // Init Temp UART
    uart_init( uart0, UART_BAUD_RATE );

    //  Using alternate pins because I'm using the Pimoroni Tiny as efficiently as I can
    gpio_set_function( UART_TX_PIN, GPIO_FUNC_UART );
    gpio_set_function( UART_RX_PIN, GPIO_FUNC_UART );

    int UART_IRQ = uart0 == uart0 ? UART0_IRQ : UART1_IRQ;

    // And set up and enable the interrupt handlers
    irq_set_exclusive_handler(UART_IRQ, on_uart_rx);
    irq_set_enabled(UART_IRQ, true);

    // Now enable the UART to send interrupts - RX only
    uart_set_irq_enables(uart0, true, false);


    // Init I2C
    i2c_init( i2c1, 1000000);
    gpio_set_function( I2C_SDA, GPIO_FUNC_I2C );
    gpio_set_function( I2C_SCL, GPIO_FUNC_I2C );
    gpio_pull_up( I2C_SDA );
    gpio_pull_up( I2C_SCL );

    // Wait for i2c to settle down before initializing the display (otherwise it won't function correctly)
    sleep_ms( 250 );

    // Init Display
    pico_ssd1306::SSD1306 display = pico_ssd1306::SSD1306( i2c1, 0x3C, pico_ssd1306::Size::W128xH32 );
    display.setOrientation(0);
    display.setContrast( 64 );

    // Init Focuser functions
    Focuser focuser;

    pico_sht3x::SHT3X TempHumid( i2c1, 0x44 );

    TempHumid.init();

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
            //printf( "LED ON!\n" );

            //uart_puts( uart0, "Hello world 1!" );

            //focuser.setTemp( 23.4 );
            //focuser.setTargetPosition( 45 );

            TempHumid.read_c( &Temp, &Humid );

            focuser.setTemp( Temp );
            focuser.setHumid( Humid );

            focuser.updateScreen( display );


            sleep_ms(1000);


            if( rxValid )
            {
                uart_puts( uart0, rxBuff );
                rxCount = 0;
                rxValid = false;
            }


            //gpio_put(G_LED_PIN, 0);
            //printf( "LED OFF!\n" );

            //uart_puts( uart0, "Hello world 2!" );

            //focuser.setTemp( 67.1 );
            //focuser.setTargetPosition( 200 );

            //focuser.updateScreen( display );
            //sleep_ms(1000);
        }

    #endif
}


void on_uart_rx()
{
    uint8_t ch = uart_getc( uart0 );

    if( ch == ':' )
    {
        // Reset receive
        rxCount = 0;
    }
    else if( ch == '#' )
    {
        rxValid = true;
    }
    else if( rxCount < 10 )
    {
        rxBuff[rxCount++] = char(ch);
        //rxCount++;
    }
    else
    {
        rxCount = 0;
    }

}

void rx_interp(  )
{

}