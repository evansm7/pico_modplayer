cmake_minimum_required(VERSION 3.16)

include(pico_sdk_import.cmake)

project(pmp C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(pmp pmp.c lmp/littlemodplayer.c)

pico_generate_pio_header(pmp ${CMAKE_CURRENT_LIST_DIR}/pio_i2s.pio)

target_link_libraries(pmp
	pico_stdlib
        hardware_dma
        hardware_irq
        hardware_pio
        )

# create map/bin/hex file etc.
pico_add_extra_outputs(pmp)
