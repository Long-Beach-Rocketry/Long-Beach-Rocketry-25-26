add_library(nanopb_test_bsp STATIC
    nanopb_app_bsp.cc
)

target_include_directories(nanopb_test_bsp PUBLIC
    .
    ../
    ${CMAKE_SOURCE_DIR}/external/nanopb
)

target_link_libraries(nanopb_test_bsp
    PUBLIC
        driver
)