add_library(nanopb STATIC
    nanopb/pb_encode.c
    nanopb/pb_decode.c
    nanopb/pb_common.c
)

# On Unix, prefer the packaged protoc binary over nanopb's wrapper script.
# Windows Git checkouts can give that script CRLF line endings, which makes its
# /usr/bin/env shebang fail when the workspace is mounted into Linux.
if(CMAKE_HOST_UNIX)
    find_program(LBR_SYSTEM_PROTOC protoc PATHS /usr/bin /usr/local/bin NO_DEFAULT_PATH REQUIRED)
    set(nanopb_PROTOC_PATH "${LBR_SYSTEM_PROTOC}" CACHE FILEPATH "System protoc executable" FORCE)
endif()
add_subdirectory(nanopb)

# Disable pthread for embedded/STM32 environments
if(NOT "${TARGET_DEVICE}" MATCHES "^(NATIVE|SITL)$")
    set(gtest_disable_pthreads ON CACHE BOOL "" FORCE)
    set(GTEST_HAS_PTHREAD 0 CACHE BOOL "" FORCE)
else()
    find_package(Threads REQUIRED)
endif()

if("${TARGET_DEVICE}" MATCHES "^(NATIVE|SITL)$")
  # For Windows: Prevent overriding the parent project's compiler/linker settings
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

  # C++ standard
  set(CMAKE_CXX_STANDARD 20)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)

  # C standard
  set(CMAKE_C_STANDARD 23)
  set(CMAKE_C_STANDARD_REQUIRED ON)
  set(CMAKE_C_EXTENSIONS OFF)

  add_subdirectory(googletest)
endif()
