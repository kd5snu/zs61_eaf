// Focuser.h
#ifndef FOCUSER_H
#define FOCUSER_H

#include <stdio.h>
#include <string.h>

#include <string>

#include "pico/stdlib.h"

#include "pico-ssd1306/ssd1306.h"
#include "pico-ssd1306/textRenderer/TextRenderer.h"

#include "hardware/i2c.h"

class Focuser
{
    private:
        float temperature;
        float humidity;
        uint32_t targetPos;
        uint32_t currPos;
    public:
        Focuser();
        Focuser(
            float t,
            float h,
            uint32_t tPos,
            uint32_t cPos );
        
        void setTemp( float );
        void setHumid( float );
        uint32_t getTargetPosition();
        void setTargetPosition( uint32_t );
        uint32_t getCurrentPosition();
        void setCurrentPosition( uint32_t );

        void updateScreen( pico_ssd1306::SSD1306 );

};



# endif