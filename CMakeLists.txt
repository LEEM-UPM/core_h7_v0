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

add_executable(${CMAKE_PROJECT_NAME}
    Core/Inc/ASPID_FDCAN_NETWORK.h)

add_subdirectory(cmake/stm32cubemx)
add_subdirectory(third_party/middlewares/threadx)
add_subdirectory(third_party/middlewares/filex)
add_subdirectory(bsp)

target_sources(${CMAKE_PROJECT_NAME} PRIVATE
<<<<<<< Updated upstream
    poc/usd_filex_dma/main.c
=======
    # poc/usd_filex_dma/main.c
    # poc/blinking_led.c
    poc/core_rf_fdcan_test.c
    Core/Src/ASPID_FDCAN_NETWORK.c
>>>>>>> Stashed changes
)

target_link_libraries(${PROJECT_NAME}
    bsp_filex_sd_driver
    threadx
    filex
    stm32cubemx
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)