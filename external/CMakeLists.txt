add_library(nanopb STATIC
    nanopb/pb_encode.c
    nanopb/pb_decode.c
    nanopb/pb_common.c
)

add_subdirectory(nanopb)

if("${TARGET_DEVICE}" STREQUAL "NATIVE")
    add_subdirectory(googletest)
endif()
