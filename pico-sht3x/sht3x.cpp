#include "sht3x.h"

namespace pico_sht3x {
    SHT3X::SHT3X( i2c_inst *i2CInst, uint8_t Address ) {
        this->i2CInst = i2CInst;
        this->address = Address;
    }

    void SHT3X::init_2hz() {
        // init with 2 measurements per second, high repeatability
        cmd_buffer[0] = 0x22;
        cmd_buffer[1] = 0x36;

        i2c_write_blocking( this->i2CInst, this->address, cmd_buffer, 2, false );
    }

    void SHT3X::read_raw( uint32_t *temp, uint32_t *humid )
    {
        cmd_buffer[0] = 0xe0;
        cmd_buffer[1] = 0x00;

        uint8_t buf[6];

        i2c_write_blocking( this->i2CInst, this->address, cmd_buffer, 2, true );
        i2c_read_blocking( this->i2CInst, this->address, buf, 6, false );

        *temp = (buf[0] << 8) + buf[1];
        *humid = (buf[3] << 8) + buf[4];
    }

    void SHT3X::read_f( float *temp, float *humid )
    {
        uint32_t Temp, Humid;

        read_raw ( &Temp, &Humid );

        *temp = -49 + (315 * (float(Temp) / 65535));
        *humid = 100 * (float(Humid) / 65535);
    }

    void SHT3X::read_c( float *temp, float *humid )
    {
        uint32_t Temp, Humid;

        read_raw ( &Temp, &Humid );

        *temp = -45 + (175 * (float(Temp) / 65535));
        *humid = 100 * (float(Humid) / 65535);
    }

}