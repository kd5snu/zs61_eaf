add_library(pico_sht3x
        sht3x.cpp)

target_link_libraries(pico_sht3x
        hardware_i2c
        pico_stdlib
        )
target_include_directories (pico_sht3x PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})