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
    /// @class SHT3X sht3x.h "pico-sht3x/sht3x.h"
    /// @brief SHT3X class represents connection to SHT3X temperature and humidity sensor
    class SHT3X {
    private:
        // I2C unit which this will be assigned to use
        i2c_inst *i2CInst;
        // I2C address. Default is 0x44, but can also use 0x45 with a jumper
        uint8_t address;

        // Temporary buffer space for command values
        uint8_t cmd_buffer[2];
        // Temporary buffer space for received response data
        uint8_t resp_buffer[6];

    public:
        SHT3X();

        /// @brief SHT3X initial setup function.
        /// @param i2CInst i2c instance. Either i2c0 or i2c1.
        /// @param Address (optional) display i2c address. defaults to 0x44, but can be 0x45 with jumper.
        //SHT3X( i2c_inst &_i2CInst, uint8_t Address=0x44 );
        SHT3X( i2c_inst *i2CInst, uint8_t Address=0x44 );

        /// @brief Ask for update rate at 2 Hz, with high reliability.
        void init_2hz();

        /// @brief Read the data back in a raw format. Useful if you want to parse or verify the data yourself.
        /// @param temp temperature variable to modify. Note that this will be a raw unsigned integer value.
        /// @param humid humidity variable to modify. Note that this will be a raw unsigned integer value.
        void read_raw( uint32_t &temp, uint32_t &humid );
        //void read_raw( uint32_t, uint32_t );

        /// @brief Reads back temperature and humidity data, presented in ºF.
        /// @param temp temperature variable to modify.
        /// @param humid humidity variable to modify.
        void read_f( float &temp, float &humid );

        /// @brief back temperature and humidity data, presented in ºC.
        /// @param temp temperature variable to modify.
        /// @param humid humidity variable to modify.
        void read_c( float &temp, float &humid );
    };
}

#endif //SHT3X_SHT3X_HPP