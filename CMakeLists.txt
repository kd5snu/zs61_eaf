cmake_minimum_required(VERSION 3.24)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

project( focuser C CXX ASM )
set( CMAKE_C_STANDARD 11 )
set( CMAKE_CXX_STANDARD 17 )

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.3.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

set(FOCUSER_PATH ${PROJECT_SOURCE_DIR})

set( PICO_BOARD "pimoroni_tiny2040")

# Initialize the SDK
pico_sdk_init()

add_subdirectory(pico-ssd1306)
add_subdirectory(pico-sht3x)

# Add focuser source
#add_subdirectory(focuser)

add_executable(focuser
        focuser.cpp
        )

# pull in common dependencies
#target_link_libraries(focuser pico_stdlib)
target_link_libraries(focuser
    pico_ssd1306
    pico_sht3x
    pico_stdlib
    hardware_i2c)


# enable usb output, disable uart output
pico_enable_stdio_usb(focuser 0)
pico_enable_stdio_uart(focuser 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(focuser)