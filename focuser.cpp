#include "Focuser.h"

Focuser::Focuser()
{
    temperature = 0.0;
    humidity = 0.0;
    targetPos = 0;
    currPos = 0;
}

Focuser::Focuser( float t, float h, uint32_t tPos, uint32_t cPos )
{
    temperature = t;
    humidity = h;
    targetPos = tPos;
    currPos = cPos;
}

void Focuser::setTemp( float temp )
{
    temperature = temp;
}

uint32_t Focuser::getTargetPosition()
{
    return targetPos;
}

void Focuser::setTargetPosition( uint32_t position )
{
    targetPos = position;
}

uint32_t Focuser::getCurrentPosition()
{
    return currPos;
}

void Focuser::setCurrentPosition( uint32_t position )
{
    currPos = position;
}

void Focuser::updateScreen( pico_ssd1306::SSD1306 display )
{
    display.clear();

    const char tempPrefix[]     = "Temp2:   ";
    const char humidPrefix[]    = "Humid:  ";
    const char tgtPrefix[]      = "Target: ";
    const char curPrefix[]      = "Curr:   ";

    const char tempSuffix[]     = " dC";
    const char humidSuffix[]    = " %RH";

    char tempBuffer[11];
    char humidBuffer[11];
    char targetBuffer[11];
    char currentBuffer[11];

    std::snprintf( tempBuffer,    11, "%10.1f", temperature );
    std::snprintf( humidBuffer,   11, "%10.1f", humidity );
    std::snprintf( targetBuffer,  11, "%10u",   targetPos );
    std::snprintf( currentBuffer, 11, "%10u",   currPos );

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