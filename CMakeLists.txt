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
    # FDCAN network 
    Core/Inc/ASPID_FDCAN_NETWORK.h
    # guidance control header files
    Code_V_1_1_2/GuidanceControl_ert_rtw/GuidanceControl_Private.h
    Code_V_1_1_2/GuidanceControl_ert_rtw/GuidanceControl_types.h
    Code_V_1_1_2/GuidanceControl_ert_rtw/GuidanceControl.h
    Code_V_1_1_2/GuidanceControl_ert_rtw/rt_nonfinite.h
    Code_V_1_1_2/GuidanceControl_ert_rtw/rtGetInf.h
    Code_V_1_1_2/GuidanceControl_ert_rtw/rtwtypes.h

    # navigation header files
    Code_V_1_1_2/Navigation_ert_rtw/Navigation_private.h
    Code_V_1_1_2/Navigation_ert_rtw/Navigation_types.h
    Code_V_1_1_2/Navigation_ert_rtw/Navigation.h
    Code_V_1_1_2/Navigation_ert_rtw/rt_defines.h
    # Code_V_1_1_2/Navigation_ert_rtw/rt_nonfinite.h
    # Code_V_1_1_2/Navigation_ert_rtw/rtGetInf.h
    Code_V_1_1_2/Navigation_ert_rtw/rtGetNaN.h
    # Code_V_1_1_2/Navigation_ert_rtw/rtwtypes.h

    # state machine header files
    Code_V_1_1_2/StateMachine_ert_rtw/StateMachine_private.h
    Code_V_1_1_2/StateMachine_ert_rtw/StateMachine_types.h
    Code_V_1_1_2/StateMachine_ert_rtw/StateMachine.h
    # Code_V_1_1_2/StateMachine_ert_rtw/rt_nonfinite.h
    # Code_V_1_1_2/StateMachine_ert_rtw/rtGetInf.h
    # Code_V_1_1_2/StateMachine_ert_rtw/rtwtypes.h
)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/poc
    ${CMAKE_CURRENT_SOURCE_DIR}/Code_V_1_1_2/GuidanceControl_ert_rtw
    ${CMAKE_CURRENT_SOURCE_DIR}/Code_V_1_1_2/Navigation_ert_rtw
    ${CMAKE_CURRENT_SOURCE_DIR}/Code_V_1_1_2/StateMachine_ert_rtw
)

add_subdirectory(cmake/stm32cubemx)
add_subdirectory(third_party/middlewares/threadx)
add_subdirectory(third_party/middlewares/filex)
add_subdirectory(bsp)

target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # poc/usd_filex_dma/main.c
    poc/gnc_software.c

    # FDCAN NETWORK source files
    Core/Src/ASPID_FDCAN_NETWORK.c

    # GuidanceControl source files
    # Code_V_1_1_2/GuidanceControl_ert_rtw/ert_main.c
    Code_V_1_1_2/GuidanceControl_ert_rtw/GuidanceControl_data.c
    Code_V_1_1_2/GuidanceControl_ert_rtw/GuidanceControl.c
    Code_V_1_1_2/GuidanceControl_ert_rtw/rt_nonfinite.c
    Code_V_1_1_2/GuidanceControl_ert_rtw/rtGetInf.c
    Code_V_1_1_2/GuidanceControl_ert_rtw/rtGetNan.c
    
    # Navigation source files
    # Code_V_1_1_2/Navigation_ert_rtw/ert_main.c
    Code_V_1_1_2/Navigation_ert_rtw/Navigation_data.c
    Code_V_1_1_2/Navigation_ert_rtw/Navigation.c
    # Code_V_1_1_2/Navigation_ert_rtw/rt_nonfinite.c
    # Code_V_1_1_2/Navigation_ert_rtw/rtGetInf.c
    # Code_V_1_1_2/Navigation_ert_rtw/rtGetNan.c

    # StateMachine source files
    # Code_V_1_1_2/StateMachine_ert_rtw/ert_main.c
    # Code_V_1_1_2/StateMachine_ert_rtw/rt_nonfinite.c
    # Code_V_1_1_2/StateMachine_ert_rtw/rtGetInf.c
    Code_V_1_1_2/StateMachine_ert_rtw/StateMachine.c
)

target_link_libraries(${PROJECT_NAME}
    bsp_filex_sd_driver
    threadx
    filex
    stm32cubemx
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)