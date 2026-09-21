cmake_minimum_required(VERSION 3.22)

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

set(CMAKE_PROJECT_NAME core_h7_v0)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# 2. Global Configuration Definition
add_library(project_config INTERFACE)
target_include_directories(project_config INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/config)
target_compile_definitions(project_config INTERFACE 
    TX_INCLUDE_USER_DEFINE_FILE
    FX_INCLUDE_USER_DEFINE_FILE
)

add_executable(${CMAKE_PROJECT_NAME})

add_subdirectory(cmake/stm32cubemx)
add_subdirectory(third_party/middlewares/threadx)
add_subdirectory(third_party/middlewares/filex)
# bsp/filex (driver SD con ThreadX) solo lo usa la PoC poc/usd_filex_dma;
# el ordenador de vuelo usa Drivers/Sd_card con FileX standalone.
#add_subdirectory(bsp)
add_subdirectory(third_party/can_protocol)

target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    #poc/can_tx_rx_rtos/main.c
    #poc/can_tx_rx_rtos/can_rx_thread.c
    #poc/can_tx_rx_rtos/can_shared_data.c
    #poc/can_tx_rx_rtos/can_tx_test.c
    # App: lógica del ordenador de vuelo
    App/flight_software.c
    App/sensor_board.c
    App/sys_utilities.c
    App/sys_power.c
    App/sys_avionics.c
    App/sys_rf.c
    App/debug_console.c
    App/storage.c
    App/sounds.c
    App/led_show.c
    # Drivers propios
    Drivers/Buzzer/buzzer.c
    Drivers/Leds/leds.c
    Drivers/Can_driver/can_driver.c
    Drivers/Debug_out/debug_out.c
    Drivers/Sd_card/sd_card.c
    Drivers/Sd_card/sd_card_fx_driver.c
    Drivers/Usb_cdc/usb_cdc.c
    Drivers/Usb_cdc/usbd_conf.c
    Drivers/Usb_cdc/usbd_desc.c
    # Librería USB Device de ST (núcleo + clase CDC)
    third_party/stm32_usb_device_library/Core/Src/usbd_core.c
    third_party/stm32_usb_device_library/Core/Src/usbd_ctlreq.c
    third_party/stm32_usb_device_library/Core/Src/usbd_ioreq.c
    third_party/stm32_usb_device_library/Class/CDC/Src/usbd_cdc.c
)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    App
    Drivers/Buzzer
    Drivers/Can_driver
    Drivers/Debug_out
    Drivers/Leds
    Drivers/Sd_card
    Drivers/Usb_cdc
    third_party/stm32_usb_device_library/Core/Inc
    third_party/stm32_usb_device_library/Class/CDC/Inc
)

# newlib-nano no formatea %f sin esto (consola de debug)
target_link_options(${CMAKE_PROJECT_NAME} PRIVATE -u _printf_float)

target_link_libraries(${PROJECT_NAME}
    threadx
    filex
    stm32cubemx
    can_protocol
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)