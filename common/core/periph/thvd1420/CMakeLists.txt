add_library(rs485 INTERFACE)

target_include_directories(rs485 INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Only link the embedded hardware driver binary layer if we aren't testing on host native
if(NOT "${TARGET_DEVICE}" MATCHES "NATIVE")
    target_link_libraries(rs485 INTERFACE
        driver
    )
endif()

# Host-native unit testing engine block
if("${TARGET_DEVICE}" MATCHES "NATIVE")
    add_executable(rs485_test
        rs485_test.cc
    )

    target_link_libraries(rs485_test
        PRIVATE
            rs485
            GTest::gtest_main
            pthread
    )

    # Explicitly supply the include paths for your abstract driver definitions
    target_include_directories(rs485_test
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/../../../drivers/io
    )

    add_test(NAME rs485_test COMMAND rs485_test)
endif()