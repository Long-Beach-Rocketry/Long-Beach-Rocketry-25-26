add_library(inverse INTERFACE)

target_include_directories(inverse INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/interface
)

if(TARGET_DEVICE MATCHES "SITL")
    add_subdirectory(hw)
    target_link_libraries(inverse INTERFACE inverse_drivers_hw)
endif()