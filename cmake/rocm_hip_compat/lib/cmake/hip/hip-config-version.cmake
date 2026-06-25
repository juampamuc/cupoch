# Version-compat shim for stdgpu's `find_package(hip 5.1 REQUIRED)`.
# stdgpu 1.3.0 pins the ROCm-5-era hip 5.1; ROCm 7's real hip-config-version
# uses SameMajorVersion compatibility, so 7.x is rejected against a 5.x request
# even though 7 > 5. This shim (selected only via cupoch's CMAKE_PREFIX_PATH
# prepend on the HIP build) reports any request as compatible; the real hip
# targets (hip::host/hip::device) are already provided by cupoch's top-level
# find_package(hip REQUIRED) before stdgpu is added.
set(PACKAGE_VERSION "7.2.53211")
set(PACKAGE_VERSION_COMPATIBLE TRUE)
if(PACKAGE_FIND_VERSION STREQUAL PACKAGE_VERSION)
    set(PACKAGE_VERSION_EXACT TRUE)
endif()
