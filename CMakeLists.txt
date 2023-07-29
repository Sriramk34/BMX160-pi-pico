cmake_minimum_required(VERSION 3.12)

include(pico_sdk_import.cmake)

project(example C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(example  
    example.c
    bmx160.c
    bmx_defines.h
)

target_link_libraries(example
    pico_stdlib
    hardware_i2c
)

SET(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -ffunction-sections -fdata-sections")
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -ffunction-sections -fdata-sections")

# Configure Pico
pico_enable_stdio_usb(example 1)
pico_enable_stdio_uart(example 0)

# Generate UF2
pico_add_extra_outputs(example)