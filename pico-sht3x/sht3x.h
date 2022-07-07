#pragma once

#ifndef SHT3X_SHT3X_H
#define SHT3X_SHT3X_H

#include <string.h>
#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "pico/stdlib.h"
/*
// Addresses
#define ADDR_DEF            = _u(0x44)
#define ADDR_ALT            = _u(0x45)

// Single shot measurement commands
#define SS_STRETCH          = _u(0x2c)
#define SS_STRETCH_HIGH     = _u(0x06)
#define SS_STRETCH_MED      = _u(0x0d)
#define SS_STRETCH_LOW      = _u(0x10)

#define SS_NOSTRETCH        = _u(0x24)
#define SS_NOSTRETCH_HIGH   = _u(0x00)
#define SS_NOSTRETCH_MED    = _u(0x0b)
#define SS_NOSTRETCH_LOW    = _u(0x16)
*/


namespace pico_sht3x
{
    class SHT3X {
    private:
        i2c_inst *i2CInst;
        uint8_t address;

        uint8_t cmd_buffer[2];
        uint8_t resp_buffer[6];

    public:
        SHT3X( i2c_inst *i2CInst, uint8_t Address );

        void init();
        void read_raw( int32_t* temp, int32_t* humid );
    };
}

#endif //SHT3X_SHT3X_HPP