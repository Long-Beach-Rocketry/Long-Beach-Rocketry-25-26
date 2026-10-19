find_package(Eigen3 REQUIRED)

add_library(kalman_filter INTERFACE)

target_sources(kalman_filter INTERFACE
    kalman.cc
)

target_include_directories(kalman_filter INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(kalman_filter INTERFACE
    Eigen3::Eigen
)