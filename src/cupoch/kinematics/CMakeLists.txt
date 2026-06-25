file(GLOB_RECURSE KINEMATICS_SOURCE_FILES "*.cpp")

# Create object library
add_library(cupoch_kinematics ${KINEMATICS_SOURCE_FILES})
if (USE_HIP)
    # These .cpp transitively include cupoch headers that pull in rocThrust ->
    # rocPRIM, which only the HIP (clang) compiler can parse; the default CXX
    # compiler (g++) cannot. Mark every TU LANGUAGE HIP. (On NVIDIA, CUDA Thrust
    # compiles fine under the host compiler, so the CUDA build is unchanged.)
    set_source_files_properties(${KINEMATICS_SOURCE_FILES} PROPERTIES LANGUAGE HIP)
    set_target_properties(cupoch_kinematics PROPERTIES HIP_ARCHITECTURES "${CMAKE_HIP_ARCHITECTURES}")
    target_link_libraries(cupoch_kinematics hip::host)
endif ()
target_link_libraries(cupoch_kinematics cupoch_collision
                      cupoch_utility
                      ${3RDPARTY_LIBRARIES})