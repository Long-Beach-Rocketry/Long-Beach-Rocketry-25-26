function(add_executable_for DEVICE EXECUTABLE LINKER_SCRIPT)
    if ("${TARGET_DEVICE}" MATCHES "${DEVICE}")
        message("Adding executable: ${EXECUTABLE}")
        message("Target: ${DEVICE}")
        add_executable(${EXECUTABLE} ${ARGN})

        if ("${TARGET_DEVICE}" MATCHES "(STM32)")
            message("Selecting linker opts for ARM MCU")
            target_link_options(${EXECUTABLE} PRIVATE
                -T${LINKER_SCRIPT}
                -specs=nosys.specs
                -lc
                -lm
                -lnosys
                -Wl,-Map=${EXECUTABLE}.map,--cref
                -Wl,--gc-sections
                # -Wl, --print-memory-usage
            )

            add_custom_command(TARGET ${EXECUTABLE}
                POST_BUILD
                COMMAND ${TOOLCHAIN_PREFIX}objcopy -O ihex ${EXECUTABLE} ${EXECUTABLE}.hex
                COMMAND ${TOOLCHAIN_PREFIX}objcopy -O binary ${EXECUTABLE} ${EXECUTABLE}.bin
                COMMAND ${TOOLCHAIN_PREFIX}objcopy -O srec ${EXECUTABLE} ${EXECUTABLE}.elf
                COMMAND ${TOOLCHAIN_PREFIX}size --format=berkeley ${EXECUTABLE}.elf
            )
        endif()
    endif()
endfunction()

function(add_library_for DEVICE TARGET)
    if ("${TARGET_DEVICE}" MATCHES "${DEVICE}")
        add_library(${TARGET} ${ARGN})
    endif()
endfunction()

function(add_subdirectory_for DEVICE TARGET)
    if ("${TARGET_DEVICE}" MATCHES "${DEVICE}")
        add_subdirectory(${TARGET})
    endif()
endfunction()

function(target_include_directories_for DEVICE TARGET)
    if ("${TARGET_DEVICE}" MATCHES "${DEVICE}")
        target_include_directories(${TARGET} ${ARGN})
    endif()
endfunction()

function(target_link_libraries_for DEVICE TARGET)
    if ("${TARGET_DEVICE}" MATCHES "${DEVICE}")
        target_link_libraries(${TARGET} ${ARGN})
    endif()
endfunction()

function(target_link_options_for DEVICE TARGET)
    if("${TARGET_DEVICE}" MATCHES "${DEVICE}")
        target_link_options(${TARGET} ${ARGN})
    endif()
endfunction()

function(target_compile_definitions_for DEVICE TARGET)
    if ("${TARGET_DEVICE}" MATCHES "${DEVICE}")
        target_compile_definitions(${TARGET} ${ARGN})
    endif()
endfunction()

function(target_preprocess_for DEVICE EXECUTABLE SRC_FILE OUT_FILE)
    if ("${TARGET_DEVICE}" MATCHES "${DEVICE}")
        message("Preprocessing: ${SRC_FILE} -> ${OUT_FILE}")
        add_custom_command(TARGET ${EXECUTABLE}
            PRE_BUILD
            COMMAND arm-none-eabi-gcc -E -P -x c ${SRC_FILE} -o ${OUT_FILE} ${ARGN}
        )
    endif()
endfunction()

function(get_directories VAR PATH)
    set(TEMP "")
    file(GLOB_RECURSE DIRECTORIES LIST_DIRECTORIES true "${PATH}/*")
    foreach(DIR ${DIRECTORIES})
        if (IS_DIRECTORY ${DIR})
            list(APPEND TEMP ${DIR})
        endif()
    endforeach()
    set(${VAR} ${TEMP} PARENT_SCOPE)
endfunction()

function(add_tests LINK_LIB)#Changethis so you enter full file name and not append .cc so you can add tests in directoryes underneith
    if ("${TARGET_DEVICE}" MATCHES "NATIVE")

        include(CTest)
        include(GoogleTest)

        foreach(SRC_NAME ${ARGN})

            add_executable(${SRC_NAME}
                ${SRC_NAME}.cc
            )

            target_link_libraries(${SRC_NAME}
                GTest::gtest_main
                ${LINK_LIB}
            )

            gtest_discover_tests(${SRC_NAME})
            add_test(NAME ${SRC_NAME} COMMAND ${SRC_NAME})

        endforeach()
    endif()
endfunction()
