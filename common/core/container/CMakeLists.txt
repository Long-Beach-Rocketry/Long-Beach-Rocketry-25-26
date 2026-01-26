add_library(container STATIC
    ring_buffer.cc
)

target_include_directories(container PUBLIC .)

add_tests(container
    ring_buffer_test
)