cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(uart_probe)
pico_sdk_init()

add_executable(uart_probe main.c) 

target_include_directories(uart_probe PRIVATE ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(uart_probe pico_stdlib pico_stdio_usb)

pico_add_extra_outputs(uart_probe)
