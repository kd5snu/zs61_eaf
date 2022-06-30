add_executable(focuser
        focuser.c
        )

# pull in common dependencies
target_link_libraries(focuser pico_stdlib pico_ssd1306 hardware_i2c)
#target_link_libraries(focuser pico_ssd1306 hardware_i2c)


# enable usb output, disable uart output
pico_enable_stdio_usb(focuser 1)
pico_enable_stdio_uart(focuser 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(focuser)

#add_compile_options(-Wall
#        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
#        -Wno-unused-function # we have some for the docs that aren't called
#        -Wno-maybe-uninitialized
#        )