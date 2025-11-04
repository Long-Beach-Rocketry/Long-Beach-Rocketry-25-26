add_library(utils STATIC
    reg_helpers.cc
)

target_include_directories(utils PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# No dependencies needed for utils - it's a standalone utility library