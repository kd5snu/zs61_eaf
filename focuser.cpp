#include <stdio.h>

#include "pico/stdlib.h"
#include "boards/pimoroni_tiny2040.h"

#include "pico-ssd1306/ssd1306.h"
#include "pico-ssd1306/textRenderer/TextRenderer.h"

#include "hardware/i2c.h"

#include "pico-sht3x/sht3x.h"

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

int main() {

    //i2c_write_blocking( i2c0, addr, data, 2,  );

    // Init I/O
    stdio_init_all();

    // Init I2C
    i2c_init( i2c1, 1000000);
    gpio_set_function( I2C_SDA, GPIO_FUNC_I2C );
    gpio_set_function( I2C_SCL, GPIO_FUNC_I2C );
    gpio_pull_up( I2C_SDA );
    gpio_pull_up( I2C_SCL );

    sleep_ms( 250 );

    // Init Display
    pico_ssd1306::SSD1306 display = pico_ssd1306::SSD1306( i2c1, 0x3C, pico_ssd1306::Size::W128xH32 );

    display.setOrientation(0);
    display.setContrast( 64 );

    drawText( &display, font_5x8, "Temp:",     0, 0 );
    drawText( &display, font_5x8, "Humid:",    0, 8 );
    drawText( &display, font_5x8, "Tgt Pos:",  0, 16 );
    drawText( &display, font_5x8, "Cur Pos:",  0, 24 );

    display.sendBuffer(); //Send buffer to device and show on screen
    

    // #ifndef PICO_DEFAULT_LED_PIN
    // #warning blink example requires a board with a regular LED
    // #else
        const uint LED_PIN = PICO_DEFAULT_LED_PIN;
        gpio_init(LED_PIN);
        gpio_init( 18 ); // Red
        gpio_init( 20 ); // Blue
        gpio_set_dir(18, GPIO_OUT);
        gpio_set_dir(LED_PIN, GPIO_OUT);
        gpio_set_dir(20, GPIO_OUT);
        gpio_put(LED_PIN, 1);
        gpio_put( 18, 0 );
        gpio_put( 20, 1 );

        
        while (true) {
            gpio_put( 18, 1 );
            printf( "LED ON!\n" );
            sleep_ms(1000);
            gpio_put(18, 0);
            printf( "LED OFF!\n" );
            sleep_ms(1000);
        }
        



    // #endif
}