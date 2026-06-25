# Config shim paired with hip-config-version.cmake in this directory. Used only
# to satisfy stdgpu's nested `find_package(hip 5.1 REQUIRED)` on the ROCm/HIP
# build of cupoch. cupoch's top-level CMakeLists already ran the real
# find_package(hip REQUIRED), which created the hip::host / hip::device imported
# targets and set the hip_* variables; stdgpu's HIP backend only consumes those
# targets and the hip_std_17 compile feature, all of which are already present.
# So this shim merely marks the package found without re-importing anything (the
# real ROCm 7 hip-config rejects stdgpu's 5.1 request via SameMajorVersion).
if(NOT TARGET hip::host)
    # Fall back to the real ROCm hip config. Do NOT use a hardcoded Linux path
    # (/opt/rocm): on Windows the ROCm SDK is elsewhere (CMAKE_PREFIX_PATH holds
    # the correct prefix). Search CMAKE_PREFIX_PATH for the real hip-config.cmake
    # (must not be THIS shim file -- guard against recursion).
    set(_hip_compat_real_found FALSE)
    foreach(_hip_compat_prefix ${CMAKE_PREFIX_PATH})
        # Normalize to forward slashes for comparison.
        cmake_path(CONVERT "${_hip_compat_prefix}" TO_CMAKE_PATH_LIST _hip_compat_prefix_norm)
        cmake_path(CONVERT "${CMAKE_CURRENT_LIST_DIR}" TO_CMAKE_PATH_LIST _hip_shim_dir)
        if("${_hip_compat_prefix_norm}/lib/cmake/hip" STREQUAL "${_hip_shim_dir}")
            continue()  # this IS the shim directory, skip
        endif()
        set(_hip_compat_cfg "${_hip_compat_prefix}/lib/cmake/hip/hip-config.cmake")
        if(EXISTS "${_hip_compat_cfg}")
            include("${_hip_compat_cfg}")
            set(_hip_compat_real_found TRUE)
            break()
        endif()
    endforeach()
    if(NOT _hip_compat_real_found)
        # Last resort: try the standard Linux path.
        if(EXISTS "/opt/rocm/lib/cmake/hip/hip-config.cmake")
            include("/opt/rocm/lib/cmake/hip/hip-config.cmake")
        endif()
    endif()
endif()
set(hip_FOUND TRUE)
