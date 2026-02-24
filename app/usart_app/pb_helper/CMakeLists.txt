add_library(pb_helper STATIC usart_pb_helper.cc)

target_include_directories(pb_helper PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/common/drivers/bus
)

target_link_libraries(pb_helper PUBLIC cmd_msg)
