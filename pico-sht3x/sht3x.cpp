#include "sht3x.h"

namespace pico_sht3x {
    SHT3X::SHT3X( i2c_inst *i2CInst, uint8_t Address ) {
        this->i2CInst = i2CInst;
        this->address = Address;
    }

    void SHT3X::init() {
        // init with 2 measurements per second, high repeatability
        cmd_buffer[0] = 0x22;
        cmd_buffer[1] = 0x36;

        i2c_write_blocking( i2c0, this->address, cmd_buffer, 2, false );
    }

    void SHT3X::read_raw( int32_t* temp, int32_t* humid )
    {
        cmd_buffer[0] = 0xe0;
        cmd_buffer[1] = 0x00;

        uint8_t buf[6];

        i2c_write_blocking( i2c0, this->address, cmd_buffer, 2, true );
        i2c_read_blocking( i2c0, this->address, buf, 6, false );

        // for( int i = 0; i < 6; i++ )
        // {
        //     pico::printf( resp_buffer[i] );
        // }
    }
}