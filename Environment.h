// environment.h
#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <stdio.h>
#include <string.h>

#include <string>

#include "pico/stdlib.h"
#include "hardware/i2c.h"

class Environment
{
    private:
        uint32_t rawTemp;
        uint32_t rawHumid;

    public:
        Environment();
};









#endif