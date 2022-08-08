#include "Focuser.h"

#include <stdio.h>

//#include "pico/stdlib.h"
#include "boards/pimoroni_tiny2040.h"

//#include "pico-ssd1306/ssd1306.h"
//#include "pico-ssd1306/textRenderer/TextRenderer.h"

//#include "hardware/i2c.h"
//#include "hardware/uart.h"

// Include custom Temp/Humid library
//#include "pico-sht3x/sht3x.h"

// Define pins for UART (only for intitial debugging)
#define UART_TX_PIN 16
#define UART_RX_PIN 17

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

//void updateScreen( pico_ssd1306::SSD1306, uint8_t, char* );

int main() {

    //i2c_write_blocking( i2c0, addr, data, 2,  );

    // Init I/O
    stdio_init_all();

    // Init Temp UART
    //  Using alternate pins because I'm using the Pimoroni Tiny as efficiently as I can
    gpio_set_function( UART_TX_PIN, GPIO_FUNC_UART );
    gpio_set_function( UART_RX_PIN, GPIO_FUNC_UART );

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

        while (true) {
            gpio_put( G_LED_PIN, 1 );
            printf( "LED ON!\n" );

            focuser.setTemp( 23.4 );
            focuser.setTargetPosition( 45 );

            focuser.updateScreen( display );


            sleep_ms(1000);


            gpio_put(G_LED_PIN, 0);
            printf( "LED OFF!\n" );

            focuser.setTemp( 67.1 );
            focuser.setTargetPosition( 200 );

            focuser.updateScreen( display );
            sleep_ms(1000);
        }

    #endif
}
