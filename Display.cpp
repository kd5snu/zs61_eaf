#include "Display.h"

namespace display {
    Display::Display() {
        this->temperature = 0.0;
        this->humidity = 0.0;
        this->targetPos = 0;
        this->currPos = 0;
    }

    void Display::setTemp( float temp )
    {
        this->temperature = temp;
    }

    void Display::setHumid( float humid )
    {
        this->humidity = humid;
    }

    uint32_t Display::getTargetPosition()
    {
        return this->targetPos;
    }

    void Display::setTargetPosition( uint32_t position )
    {
        this->targetPos = position;
    }

    uint32_t Display::getCurrentPosition()
    {
        return this->currPos;
    }

    void Display::setCurrentPosition( uint32_t position )
    {
        this->currPos = position;
    }

    void Display::updateScreen( pico_ssd1306::SSD1306 display )
    {
        display.clear();

        const char tempPrefix[]     = "Temp:   ";
        const char humidPrefix[]    = "Humid:  ";
        const char tgtPrefix[]      = "Target: ";
        const char curPrefix[]      = "Curr:   ";

        const char tempSuffix[]     = " dC";
        const char humidSuffix[]    = " %RH";

        char tempBuffer[11];
        char humidBuffer[11];
        char targetBuffer[11];
        char currentBuffer[11];

        std::snprintf( tempBuffer,    11, "%10.1f", this->temperature );
        std::snprintf( humidBuffer,   11, "%10.1f", this->humidity );
        std::snprintf( targetBuffer,  11, "%10u",   this->targetPos );
        std::snprintf( currentBuffer, 11, "%10u",   this->currPos );

        std::string tempText    = std::string( tempPrefix )  + tempBuffer  + tempSuffix;
        std::string humidText   = std::string( humidPrefix ) + humidBuffer + humidSuffix;
        std::string targetText  = std::string( tgtPrefix )   + targetBuffer;
        std::string currentText = std::string( curPrefix )   + currentBuffer;

        drawText( &display, font_5x8, tempText.c_str(),     0,  0 );
        drawText( &display, font_5x8, humidText.c_str(),    0,  8 );
        drawText( &display, font_5x8, targetText.c_str(),   0, 16 );
        drawText( &display, font_5x8, currentText.c_str(),  0, 24 );

        display.sendBuffer();
    }
}