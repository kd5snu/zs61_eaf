#include "Environment.h"

namespace environment {
        
    Environment::Environment( const pico_sht3x::SHT3X _sensor ){
    //Environment::Environment(){
        this->temperature = 0.0;
        this->humidity = 0.0;
        this->dewPoint = 0.0;

        this->sensor = _sensor;
    }

    float Environment::getTemperature()
    {
        return this->temperature;
    }

    float Environment::getHumidity()
    {
        return this->humidity;
    }

    float Environment::getDewPoint()
    {
        return this->dewPoint;
    }

    void Environment::updateSensors() // pico_sht3x::SHT3X &sensor )
    {
        this->sensor.read_c( this->temperature, this->humidity );
    }
}