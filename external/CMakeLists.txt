# Fetch Google Test
include(FetchContent)

# Disable pthread for embedded/STM32 environments
if(LBR_IS_MCU_BUILD)
    set(gtest_disable_pthreads ON CACHE BOOL "" FORCE)
    set(GTEST_HAS_PTHREAD 0 CACHE BOOL "" FORCE)
else()
    find_package(Threads REQUIRED)
endif()

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.17.0
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

#add_subdirectory(nanopb)