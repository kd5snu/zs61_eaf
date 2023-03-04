// Messages.h
#ifndef UART_H
#define UART_H

#include <stdio.h>
#include <string.h>

#include <string>

#include "pico/stdlib.h"
#include "hardware/uart.h"


class Comms
{
    private:
        char rxBuffer[16];
        char txBuffer[16];

        uint8_t rxCount;
        uint8_t txCount;

        bool rxValid;

    public:
        Comms();

        void uart_rx_isr();
        void commsCheck();
};











#endif