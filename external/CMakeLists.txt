# Disable pthread for embedded/STM32 environments
if(NOT "${TARGET_DEVICE}" STREQUAL "NATIVE")
    set(gtest_disable_pthreads ON CACHE BOOL "" FORCE)
    set(GTEST_HAS_PTHREAD 0 CACHE BOOL "" FORCE)
else()
    find_package(Threads REQUIRED)
endif()

if("${TARGET_DEVICE}" MATCHES "NATIVE")
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

#add_subdirectory(nanopb)