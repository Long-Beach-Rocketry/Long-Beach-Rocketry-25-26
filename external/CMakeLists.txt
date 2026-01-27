# Fetch Google Test
include(FetchContent)

find_package(Threads REQUIRED)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.17.0
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Disable pthread for embedded/STM32 environments
set(GTEST_HAS_PTHREAD 0 CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

#add_subdirectory(nanopb)