# The parent visits this folder for every device. Create the host executable
# only for NATIVE/SITL, including when TARGET_APP=noise_model_sim_test is used.
if("${TARGET_DEVICE}" MATCHES "^(NATIVE|SITL)$")
    add_executable(noise_model_sim main.cc)
    target_link_libraries(noise_model_sim PRIVATE sitl_noise)
endif()
