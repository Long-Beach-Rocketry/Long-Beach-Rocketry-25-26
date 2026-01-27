add_library(container INTERFACE)

target_include_directories(container INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)

if(BUILD_TESTING AND NOT LBR_IS_MCU_BUILD)
    add_executable(ring_buffer_test
        ring_buffer_test.cc
    )

    target_link_libraries(ring_buffer_test
        PRIVATE
            container
            GTest::gtest_main
    )

    add_test(NAME ring_buffer_test COMMAND ring_buffer_test)
endif()