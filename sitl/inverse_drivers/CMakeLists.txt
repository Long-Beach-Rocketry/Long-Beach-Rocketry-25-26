add_library(inverse_drivers INTERFACE)

target_include_directories(inverse_drivers INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/interface
)

if(TARGET_DEVICE MATCHES "SITL")
    add_subdirectory(hw)
    target_link_libraries(inverse_drivers INTERFACE inverse_drivers_hw)
endif()