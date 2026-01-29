cmake_minimum_required(VERSION 3.26)

project(myapp
    LANGUAGES CXX C ASM
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)

# Disable pthread for GoogleTest on MCU builds
if(NOT "${TARGET_DEVICE}" STREQUAL "NATIVE")
    set(gtest_disable_pthreads ON CACHE BOOL "" FORCE)
    add_compile_definitions(GTEST_HAS_PTHREAD=0)
    # Disable building GoogleTest for MCU Targets
    set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
    set(BUILD_GTEST OFF CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
endif()

enable_testing()

include(cmake/build-util.cmake)

if(MCU_NAME MATCHES "^(STM32).*")
    message("Found STM32 Device!")
    add_compile_definitions(
        ${MCU_NAME}=TRUE
        ${MCU_FAMILY}=TRUE
        USE_HAL_DRIVER=TRUE
    )
endif()

if ("${TARGET_APP}" STREQUAL "")
    add_subdirectory(app)
else()
    add_subdirectory(app/${TARGET_APP})
endif()

add_subdirectory(common)
add_subdirectory(external)
add_subdirectory(mcu_support)