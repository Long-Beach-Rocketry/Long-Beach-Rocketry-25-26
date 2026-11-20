add_library(atmosphere STATIC
    atmosphere.cc
)
 
target_include_directories(atmosphere PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)