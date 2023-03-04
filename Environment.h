// Environment.h
#pragma once

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H


#include <stdio.h>
#include <string.h>

#include <string>

#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include "pico-sht3x/sht3x.h"

namespace environment
{
    /// @class Environment Environment.h "Environment.h"
    /// @brief Environment class represents the current temperature, humidity, and dew point.
    class Environment {

        private: 
            float temperature;
            float humidity;
            float dewPoint;

            pico_sht3x::SHT3X sensor;

        public:
            /// @brief Environment initial setup function
            Environment( const pico_sht3x::SHT3X _sensor);
            //Environment();

            /// @brief Return the most recently saved temperature.
            /// @return Recent temperature measurement.
            float getTemperature();

            /// @brief Return the most recently saved humidity.
            /// @return Recent humidity measurement.
            float getHumidity();

            /// @brief Return the most recently calculated dew point. This is the
            ///         inaccurate way to do it.
            /// @return Recent dew point calculation.
            float getDewPoint();

            /// @brief Pulls a new set of temperature and humidity from the sensor.
            /// @param pico_sht3x::SHT3X Object reference to the temperature and humidity sensor which will be used.
            //void updateSensors( pico_sht3x::SHT3X & );
            void updateSensors();
    };
}

# endif