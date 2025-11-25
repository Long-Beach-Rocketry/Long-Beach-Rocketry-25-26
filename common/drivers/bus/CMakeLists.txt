add_library(comm STATIC
    i2c.cc
)

target_include_directories(comm PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
