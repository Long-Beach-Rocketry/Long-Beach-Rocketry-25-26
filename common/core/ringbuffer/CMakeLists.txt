add_library(ringbuffer STATIC
    ringbuffer.cc
)

target_include_directories(ringbuffer PUBLIC .)

add_tests(ringbuffer
    ring_buffer_test
)