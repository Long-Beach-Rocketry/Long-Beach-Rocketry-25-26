add_library(sitl_noise STATIC noise_model.cc)

target_include_directories(sitl_noise
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

if("${TARGET_DEVICE}" MATCHES "^(NATIVE|SITL)$")
    add_executable(sitl_noise_test noise_model_test.cc)
    target_link_libraries(sitl_noise_test
        PRIVATE
            sitl_noise
            GTest::gtest_main
    )
    add_test(NAME sitl_noise_test COMMAND sitl_noise_test)
endif()
