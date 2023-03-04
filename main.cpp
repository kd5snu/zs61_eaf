#include "Display.h"
#include "Environment.h"
//#include "Comms.h"

#include <stdio.h>
#include <iostream>
#include <string.h>
#include <string>

#include "pico/stdlib.h"
#include "boards/pimoroni_tiny2040.h"

//#include "hardware/i2c.h"
#include "hardware/uart.h"
#include "hardware/irq.h"
#include "hardware/timer.h"

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

//uint32_t rxCount;
char rxBuff[16];
bool rxValid = false;
static int chars_rxed = 0;
void on_uart_rx();


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


// Setup for timer portion
#define TIMER_PIN   15

#define ALARM0_NUM   0
#define ALARM1_NUM   1
#define ALARM2_NUM   2

#define ALARM0_IRQ TIMER_IRQ_0
#define ALARM1_IRQ TIMER_IRQ_1
#define ALARM2_IRQ TIMER_IRQ_2

static volatile bool alarm0_fired;
static volatile bool alarm1_fired;
static volatile bool alarm2_fired;

static void alarm0_irq( void );
static void alarm1_irq( void );
static void alarm_in_us( uint8_t alarm, uint32_t delay_us );

// Updates the display and temperature sensors. Usually called every second.
void displayUpdate_irq( void );
bool updateDisplay              = 0;

//void displayUpdate( environment::Environment, pico_sht3x::SHT3X, display::Display, pico_ssd1306::SSD1306 );
void displayUpdate( environment::Environment &, display::Display &, pico_ssd1306::SSD1306 & );

void commsCheck( environment::Environment & );

uint32_t    currentPosition     = 0;
uint32_t    targetPosition      = 0;
uint8_t     externalMotorSpeed  = 0;
uint16_t    internalMotorSpeed  = 0;
bool        stepMode            = 0;
bool        isMoving            = 0;
bool        startMoving         = 0;
bool        stopMoving          = 0;

uint32_t    pulseWidth          = 1;
uint32_t    pulseWidthDelay     = 0;

using namespace std;

int main() {

    //rxCount = 0;
    //rxValid = false;

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
    uart_puts(UART_ID, "Abbott Focuser v1.2\nBased off of Moonlite protocol.\n");

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

    // Init Environment functions
    environment::Environment Environment = environment::Environment( TempHumid );


    gpio_init( TIMER_PIN );
    gpio_set_dir( TIMER_PIN, GPIO_OUT );

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

        //float Temp = 0.0, Humid = 0.0;

        // Enable the interrupt for our alarm (the timer outputs 4 alarm irqs)
        hw_set_bits(&timer_hw->inte, 1u << ALARM0_NUM);
        hw_set_bits(&timer_hw->inte, 1u << ALARM1_NUM);
        hw_set_bits(&timer_hw->inte, 1u << ALARM2_NUM);

        irq_set_exclusive_handler( ALARM0_IRQ, alarm0_irq );
        irq_set_exclusive_handler( ALARM1_IRQ, alarm1_irq );
        irq_set_exclusive_handler( ALARM2_IRQ, displayUpdate_irq );

        //irq_set_enabled(ALARM0_IRQ, true);
        //irq_set_enabled(ALARM1_IRQ, true);
        irq_set_enabled(ALARM2_IRQ, true);

        alarm_in_us( 0, 50000 );

        alarm_in_us( 2, 1000000 );

        while (true) {

            // ledStatus = 1 - ledStatus;

            // gpio_put( G_LED_PIN, ledStatus );

            // TempHumid.read_c( &temperature, &humidity );
            // Environment.updateSensors( TempHumid );

            // Display.setTemp( Environment.getTemperature() );
            // Display.setHumid( Environment.getHumidity() );
            // Display.setTargetPosition( targetPosition );
            // Display.setCurrentPosition( currentPosition );

            // Display.updateScreen( oled );


            // sleep_ms(1000);



            // if( alarm0_fired )
            // {
            //     alarm0_fired = false;


            // }

            if( rxValid )
            {
                commsCheck( Environment );
            }

            if( updateDisplay )
            {
                //displayUpdate( Environment, TempHumid, Display, oled );
                displayUpdate( Environment, Display, oled );
            }
        }

    #endif
}



// Turn on the LED
static void alarm0_irq(void)
{
    //gpio_put( TIMER_PIN, 1 - gpio_get( TIMER_PIN ) );
    gpio_put( PICO_DEFAULT_LED_PIN, 1 );
    gpio_put( TIMER_PIN, 1 );

    // Alarm is only 32 bits so if trying to delay more
    // than that need to be careful and keep track of the upper
    // bits
    //uint64_t currTime = timer_hw->timerawl;
    volatile uint64_t currTime = time_us_64();

    // Clear the alarm irq
    hw_clear_bits(&timer_hw->intr, 1u << ALARM0_NUM);
    //hw_clear_bits(&timer_hw->intr, 1u << ALARM1_NUM);

    //hw_set_bits(&timer_hw->inte, 1u << ALARM0_NUM);
    //hw_set_bits(&timer_hw->inte, 1u << ALARM1_NUM);

    timer_hw->alarm[ALARM0_NUM] = (uint32_t) currTime + 35;
    timer_hw->alarm[ALARM1_NUM] = (uint32_t) currTime + pulseWidth;

    // Assume alarm 0 has fired
    //printf("Alarm IRQ fired\n");

    pulseWidthDelay++;
    alarm0_fired = true;
}

//Turn off the LED
static void alarm1_irq(void)
{
    // Clear the alarm irq
    hw_clear_bits(&timer_hw->intr, 1u << ALARM1_NUM);

    gpio_put( PICO_DEFAULT_LED_PIN, 0 );
    gpio_put( TIMER_PIN, 0 );

    // Assume alarm 0 has fired
    //printf("Alarm IRQ fired\n");
    //alarm_fired = true;
}

static void alarm_in_us( uint8_t alarm, uint32_t delay_us) {
    // Enable the interrupt for our alarm (the timer outputs 4 alarm irqs)
    //hw_set_bits(&timer_hw->inte, 1u << alarm);
    
    
    // Set irq handler for alarm irq
    //irq_set_exclusive_handler(ALARM_IRQ, alarm0_irq);
    
    
    // Enable the alarm irq
    //irq_set_enabled(ALARM_IRQ, true);
    // Enable interrupt in block and at processor

    // Alarm is only 32 bits so if trying to delay more
    // than that need to be careful and keep track of the upper
    // bits
    uint64_t target = timer_hw->timerawl + delay_us;

    // Write the lower 32 bits of the target time to the alarm which
    // will arm it
    timer_hw->alarm[alarm] = (uint32_t) target;
}

void displayUpdate_irq(void)
{
    hw_clear_bits( &timer_hw->intr, 1u << ALARM2_NUM );

    volatile uint64_t currTime = time_us_64();

    timer_hw->alarm[ALARM2_NUM] = (uint32_t) currTime + 1000000;

    updateDisplay = true;
}

//void displayUpdate( environment::Environment _Environment, pico_sht3x::SHT3X _Sensor, display::Display _Display, pico_ssd1306::SSD1306 _oled )
void displayUpdate( environment::Environment &_Environment, display::Display &_Display, pico_ssd1306::SSD1306 &_oled )
{
    //_Environment.updateSensors( _Sensor );
    _Environment.updateSensors();

    _Display.setTemp( _Environment.getTemperature() );
    _Display.setHumid( _Environment.getHumidity() );
    _Display.setTargetPosition( targetPosition );
    _Display.setCurrentPosition( currentPosition );

    _Display.updateScreen( _oled );

    updateDisplay = false;

}

void commsCheck( environment::Environment &_Environment )
{
    switch( rxBuff[1] ) {

        // Motion
        case 'F':
        {
            switch( rxBuff[2] ) {

                // Go
                case 'G':
                {
                    stopMoving = 0;
                    startMoving = 1;
                    break;
                }

                case 'Q':
                {
                    stopMoving = 1;
                    startMoving = 0;
                    break;
                }
            }
            break;
        }

        // Getters
        //else if( rxBuff[1] == 'G' )
        case 'G':
        {
            char feedback[8];

            switch( rxBuff[2] ) {

                // Motor speed
                case 'D':
                {
                    sprintf( feedback, "%x", externalMotorSpeed );
                    break;
                }

                // Step mode
                case 'H':
                {
                    sprintf( feedback, "%x", 0xff*stepMode );
                    break;
                }

                // Motion query
                case 'I':
                {
                    sprintf( feedback, "%x", isMoving );
                    break;
                }

                // Feedback target position
                case 'N':
                {
                    // :GNxxxx,xxxx#
                
                    sprintf( feedback, "%x", targetPosition );
                    break;
                }

                // Feedback current position data
                case 'P':
                {
                    // :GPxxxx,xxxx#

                    sprintf( feedback, "%x", currentPosition );
                    break;
                }

                // Feedback temperature data
                case 'T':
                {
                    // :GTxxxx,xxxx#

                    // Double the temperature, then turn it into a signed int
                    //_Environment.updateSensors();
                    sprintf( feedback, "%08x", int16_t( _Environment.getTemperature()*2 ));
                    break;
                }

                // Feedback firmware version
                case 'V':
                {
                    sprintf( feedback, "%d", 10 );
                    break;
                }
            }

            uart_puts( UART_ID, feedback );
            uart_putc( UART_ID, '#' );
            break;
        }

        // Setters
        //if( rxBuff[1] == 'S' )
        case 'S':
        {

            switch( rxBuff[2] ) {

                // Speed / Delay
                case 'D':
                {
                    uint8_t externalMotorSpeed = stoul( string(rxBuff).substr(3, chars_rxed-4), nullptr, 16 );

                    switch(externalMotorSpeed) {
                        case 0x02:
                            internalMotorSpeed = 250;
                            break;
                        case 0x04:
                            internalMotorSpeed = 125;
                            break;
                        case 0x08:
                            internalMotorSpeed = 63;
                            break;
                        case 0x10:
                            internalMotorSpeed = 32;
                            break;
                        case 0x20:
                            internalMotorSpeed = 16;
                            break;
                    }
                    break;
                }
                
                // Full step mode
                case 'F':
                {
                    stepMode = 0;
                    break;
                }

                // Half step mode
                case 'H':
                {
                    stepMode = 1;
                    break;
                }

                // Set new target position
                case 'N':
                {
                    targetPosition = 0;

                    // :SNxxxx,xxxx#
                    // Total characters - 4 gives length to extract. Start at the third place for the first digit.

                    targetPosition = stoul( string(rxBuff).substr(3, chars_rxed-4), nullptr, 16 );
                    break;
                }

                // Set new current position
                case 'P':
                {
                    // :SPxxxx,xxxx#
                    currentPosition = stoul( string(rxBuff).substr(3, chars_rxed-4), nullptr, 16 );
                    break;
                }
            }
            break;
        }
    }

    chars_rxed = 0;
    rxValid = false;
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
    }
}