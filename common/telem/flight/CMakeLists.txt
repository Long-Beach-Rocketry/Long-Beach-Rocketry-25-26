add_library(ft_app STATIC
    ft_app.cc
)

target_include_directories(flight_telem_app PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/common/core/network/pipeline
    ${PROJECT_SOURCE_DIR}/common/core/drivers
    ${PROJECT_SOURCE_DIR}/common/core/utils
)

target_link_libraries(ft_app PUBLIC
    # Link pipeline and flash dependencies
    pipeline
    w25q
)