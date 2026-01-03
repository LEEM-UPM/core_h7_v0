# ThreadX RTOS Library CMakeLists.txt
cmake_minimum_required(VERSION 3.22)

# Create ThreadX library as STATIC library
add_library(threadx STATIC)

# ThreadX port-specific assembly sources (Cortex-M7 GNU)
set(THREADX_PORT_ASM_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/ports/cortex_m7/gnu/src/tx_thread_context_restore.S
    ${CMAKE_CURRENT_SOURCE_DIR}/ports/cortex_m7/gnu/src/tx_thread_context_save.S
    ${CMAKE_CURRENT_SOURCE_DIR}/ports/cortex_m7/gnu/src/tx_thread_interrupt_control.S
    ${CMAKE_CURRENT_SOURCE_DIR}/ports/cortex_m7/gnu/src/tx_thread_interrupt_disable.S
    ${CMAKE_CURRENT_SOURCE_DIR}/ports/cortex_m7/gnu/src/tx_thread_interrupt_restore.S
    ${CMAKE_CURRENT_SOURCE_DIR}/ports/cortex_m7/gnu/src/tx_thread_schedule.S
    ${CMAKE_CURRENT_SOURCE_DIR}/ports/cortex_m7/gnu/src/tx_thread_stack_build.S
    ${CMAKE_CURRENT_SOURCE_DIR}/ports/cortex_m7/gnu/src/tx_thread_system_return.S
    ${CMAKE_CURRENT_SOURCE_DIR}/ports/cortex_m7/gnu/src/tx_timer_interrupt.S
)

# Platform-specific ThreadX sources (low-level initialization)
set(THREADX_PLATFORM_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../../platform/threadx/tx_initialize_low_level.S
)

# ThreadX common C sources
set(THREADX_COMMON_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_initialize_high_level.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_initialize_kernel_enter.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_initialize_kernel_setup.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_block_allocate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_block_pool_cleanup.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_block_pool_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_block_pool_delete.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_block_pool_info_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_block_pool_initialize.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_block_pool_prioritize.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_block_release.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_byte_allocate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_byte_pool_cleanup.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_byte_pool_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_byte_pool_delete.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_byte_pool_info_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_byte_pool_initialize.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_byte_pool_prioritize.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_byte_pool_search.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_byte_release.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_event_flags_cleanup.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_event_flags_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_event_flags_delete.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_event_flags_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_event_flags_info_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_event_flags_initialize.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_event_flags_set.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_event_flags_set_notify.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_mutex_cleanup.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_mutex_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_mutex_delete.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_mutex_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_mutex_info_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_mutex_initialize.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_mutex_prioritize.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_mutex_priority_change.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_mutex_put.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_queue_cleanup.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_queue_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_queue_delete.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_queue_flush.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_queue_front_send.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_queue_info_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_queue_initialize.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_queue_prioritize.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_queue_receive.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_queue_send.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_queue_send_notify.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_semaphore_ceiling_put.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_semaphore_cleanup.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_semaphore_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_semaphore_delete.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_semaphore_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_semaphore_info_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_semaphore_initialize.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_semaphore_prioritize.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_semaphore_put.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_semaphore_put_notify.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_thread_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_thread_delete.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_thread_entry_exit_notify.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_thread_identify.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_thread_info_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_thread_initialize.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_thread_preemption_change.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_thread_priority_change.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_thread_relinquish.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_thread_reset.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_thread_resume.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_thread_shell_entry.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_thread_sleep.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_thread_stack_analyze.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_thread_stack_error_handler.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_thread_stack_error_notify.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_thread_suspend.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_thread_system_preempt_check.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_thread_system_resume.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_thread_system_suspend.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_thread_terminate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_thread_time_slice.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_thread_time_slice_change.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_thread_timeout.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_thread_wait_abort.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_time_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_time_set.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_timer_activate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_timer_change.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_timer_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_timer_deactivate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_timer_delete.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_timer_expiration_process.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_timer_info_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_timer_initialize.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_timer_system_activate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_timer_system_deactivate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/tx_timer_thread_entry.c
)

# ThreadX error checking wrapper sources (txe_*)
set(THREADX_ERROR_CHECKING_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/txe_block_allocate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/txe_block_pool_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/txe_block_pool_delete.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/txe_block_pool_info_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/txe_block_pool_prioritize.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/txe_block_release.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/txe_byte_allocate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/txe_byte_pool_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/txe_byte_pool_delete.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/txe_byte_pool_info_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/txe_byte_pool_prioritize.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/txe_byte_release.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/txe_event_flags_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/txe_event_flags_delete.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/txe_event_flags_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/txe_event_flags_info_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/txe_event_flags_set.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/txe_event_flags_set_notify.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/txe_mutex_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/txe_mutex_delete.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/txe_mutex_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/txe_mutex_info_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/txe_mutex_prioritize.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/txe_mutex_put.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/txe_queue_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/txe_queue_delete.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/txe_queue_flush.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/txe_queue_front_send.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/txe_queue_info_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/txe_queue_prioritize.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/txe_queue_receive.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/txe_queue_send.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/txe_queue_send_notify.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/txe_semaphore_ceiling_put.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/txe_semaphore_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/txe_semaphore_delete.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/txe_semaphore_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/txe_semaphore_info_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/txe_semaphore_prioritize.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/txe_semaphore_put.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/txe_semaphore_put_notify.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/txe_thread_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/txe_thread_delete.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/txe_thread_entry_exit_notify.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/txe_thread_info_get.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/txe_thread_preemption_change.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/txe_thread_priority_change.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/txe_thread_relinquish.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/txe_thread_reset.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/txe_thread_resume.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/txe_thread_suspend.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/txe_thread_terminate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/txe_thread_time_slice_change.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/txe_thread_wait_abort.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/txe_timer_activate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/txe_timer_change.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/txe_timer_create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/txe_timer_deactivate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/txe_timer_delete.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/src/txe_timer_info_get.c
)

# ThreadX include directories
set(THREADX_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/common/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/ports/cortex_m7/gnu/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../platform/threadx  # For tx_user.h
)

target_compile_definitions(threadx INTERFACE
    TX_INCLUDE_USER_DEFINE_FILE
)

# Add all sources to the library
target_sources(threadx PRIVATE
    ${THREADX_PORT_ASM_SOURCES}
    ${THREADX_PLATFORM_SOURCES}
    ${THREADX_COMMON_SOURCES}
    ${THREADX_ERROR_CHECKING_SOURCES}
)

# Set include directories for ThreadX
target_include_directories(threadx PUBLIC
    ${THREADX_INCLUDE_DIRS}
)

