add_library(sensor_manager INTERFACE)

target_include_directories(sensor_manager INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/common/core/math
)

target_link_libraries(sensor_manager INTERFACE
    core
    driver
)

add_subdirectory_for(STM32H7 h7)

add_library(manager INTERFACE)

target_link_libraries(manager INTERFACE
    sensor_manager
)

if (TARGET st_sensor_manager)
    target_link_libraries(manager INTERFACE
        st_sensor_manager
    )
endif()
