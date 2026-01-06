# SD driver for FileX
add_library(bsp_filex_sd_driver STATIC)

target_sources(bsp_filex_sd_driver PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/filex/sd_driver/fx_stm32_sd_driver.c
    ${CMAKE_CURRENT_SOURCE_DIR}/filex/sd_driver/fx_stm32_sd_driver_glue_dma_rtos.c
)

target_include_directories(bsp_filex_sd_driver PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/filex/sd_driver
)

target_link_libraries(bsp_filex_sd_driver PUBLIC 
    filex
    stm32cubemx
)