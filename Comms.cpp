#include "Comms.h"

Comms::Comms()
{
    rxCount = 0;
    txCount = 0;

    rxValid = 0;
}


Comms::uart_rx_isr()
{
    break;
}


Comms::commsCheck()
{
    switch( this->rxBuffer[1] ) {

        // Motion
        case 'F':
        {
            switch( this->rxBuffer[2] ) {

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
        case 'G':
        {
            //char feedback[8];

            switch( this->rxBuffer[2] ) {

                // Motor speed
                case 'D':
                {
                    sprintf( this->txBuffer, "%x", externalMotorSpeed );
                    break;
                }

                // Step mode
                case 'H':
                {
                    sprintf( this->txBuffer, "%x", 0xff*stepMode );
                    break;
                }

                // Motion query
                case 'I':
                {
                    sprintf( this->txBuffer, "%x", isMoving );
                    break;
                }

                // Feedback target position
                case 'N':
                {
                    // :GNxxxx,xxxx#
                
                    sprintf( this->txBuffer, "%x", targetPosition );
                    break;
                }

                // Feedback current position data
                case 'P':
                {
                    // :GPxxxx,xxxx#

                    sprintf( this->txBuffer, "%x", currentPosition );
                    break;
                }

                // Feedback temperature data
                case 'T':
                {
                    // :GTxxxx,xxxx#

                    // Double the temperature, then turn it into a signed int
                    sprintf( this->txBuffer, "%08x", int16_t( temperature*2 ));
                    break;
                }

                // Feedback firmware version
                case 'V':
                {
                    sprintf( this->txBuffer, "%d", 10 );
                    break;
                }
            }

            uart_puts( UART_ID, this->txBuffer );
            uart_putc( UART_ID, '#' );
            break;
        }

        // Setters
        //if( rxBuff[1] == 'S' )
        case 'S':
        {

            switch( this->rxBuffer[2] ) {

                // Speed / Delay
                case 'D':
                {
                    uint8_t externalMotorSpeed = stoul( string(this->rxBuffer).substr(3, this->rxCount-4), nullptr, 16 );

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

                    targetPosition = stoul( string(this->rxBuffer).substr(3, this->rxCount-4), nullptr, 16 );
                    break;
                }

                // Set new current position
                case 'P':
                {
                    // :SPxxxx,xxxx#
                    currentPosition = stoul( string(this->rxBuffer).substr(3, this->rxCount-4), nullptr, 16 );
                    break;
                }
            }
            break;
        }
    }

    this->rxCount = 0;
    this->rxValid = false;
}