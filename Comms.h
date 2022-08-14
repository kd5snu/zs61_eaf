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
        char rxBuffer;
        char txBuffer;

        uint8_t rxCount;
        uint8_t txCount;

    public:
        Comms();
};











#endif