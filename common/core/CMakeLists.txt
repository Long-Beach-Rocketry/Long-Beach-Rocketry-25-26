add_subdirectory(periph)
add_subdirectory(utils)

# Make core consumers also get utils by adding utils to the INTERFACE core target
# `core` is defined in the parent `common/CMakeLists.txt` as an INTERFACE target.
if (TARGET core)
	target_link_libraries(core INTERFACE utils)
endif()
