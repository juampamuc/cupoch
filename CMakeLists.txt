cmake_minimum_required(VERSION 3.0.0)

if (NOT CMAKE_VERSION VERSION_LESS "3.1")
    cmake_policy(SET CMP0054 NEW)
endif()

add_custom_target(build_all_3rd_party_libs
    COMMAND ${CMAKE_COMMAND} -E echo "Custom target build_all_3rd_party_libs reached."
)

# central location for specifying the Cupoch version
file(STRINGS "src/cupoch/version.txt" CUPOCH_VERSION_READ)
foreach(ver ${CUPOCH_VERSION_READ})
  if (ver MATCHES "CUPOCH_VERSION_(MAJOR|MINOR|PATCH|TWEAK) +([^ ]+)$")
    set(CUPOCH_VERSION_${CMAKE_MATCH_1} "${CMAKE_MATCH_2}" CACHE INTERNAL "")
  endif()
endforeach()
string(CONCAT CUPOCH_VERSION "${CUPOCH_VERSION_MAJOR}"
                             ".${CUPOCH_VERSION_MINOR}"
                             ".${CUPOCH_VERSION_PATCH}"
                             ".${CUPOCH_VERSION_TWEAK}")
project(cupoch VERSION ${CUPOCH_VERSION})
message(STATUS "cupoch ${PROJECT_VERSION}")

# npm version has to be MAJOR.MINOR.PATCH
string(CONCAT PROJECT_VERSION_THREE_NUMBER "${CUPOCH_VERSION_MAJOR}"
                                           ".${CUPOCH_VERSION_MINOR}"
                                           ".${CUPOCH_VERSION_PATCH}")

# PyPI package name controls specifies the repository name on PyPI. The default
# name is "cupoch". 
if(NOT DEFINED PYPI_PACKAGE_NAME)
    set(PYPI_PACKAGE_NAME "cupoch")
endif()

# set additional info
set(PROJECT_EMAIL   "nekanat.stock@gmail.com")
set(PROJECT_HOME    "https://github.com/neka-nat/cupoch")
set(PROJECT_DOCS    "https://github.com/neka-nat/cupoch")
set(PROJECT_CODE    "https://github.com/neka-nat/cupoch")
set(PROJECT_ISSUES  "https://github.com/neka-nat/cupoch/issues")


if(WIN32 AND NOT CYGWIN)
    set(DEF_INSTALL_CMAKE_DIR CMake)
else()
    set(DEF_INSTALL_CMAKE_DIR lib/cmake/cupoch)
endif()
set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH
    "Installation directory for CMake files")

option(USE_HIP                   "Build with HIP for AMD GPUs (ROCm)"       OFF)
option(BUILD_UNIT_TESTS          "Build the Cupoch unit tests"              ON)
option(BUILD_EIGEN3              "Use the Eigen3 that comes with Cupoch"    ON)
option(BUILD_GLEW                "Build glew from source"                   OFF)
option(BUILD_GLFW                "Build glfw from source"                   OFF)
option(BUILD_JSONCPP             "Build json from source"                   OFF)
option(BUILD_PNG                 "Build png from source"                    OFF)
option(BUILD_JPEG                "Build jpeg-turbo from source"             ON)
option(BUILD_PYBIND11            "Build pybind11 from source"               ON)
option(BUILD_PYTHON_MODULE       "Build the python module"                  ON)
include(CMakeDependentOption)
# RMM (RAPIDS Memory Manager) is CUDA-only. Force it OFF (and hide it) for the HIP
# build, where the allocator falls back to thrust::device_vector
# (see utility/device_vector.h); the user never has to pass -DUSE_RMM=OFF.
cmake_dependent_option(USE_RMM   "Use rmm library(fast memory allocator)"   ON "NOT USE_HIP" OFF)
option(STATIC_WINDOWS_RUNTIME    "Use static (MT/MTd) Windows runtime"      OFF)
option(CMAKE_USE_RELATIVE_PATHS  "If true, cmake will use relative paths"   ON)

if(UNIX)
    include(GNUInstallDirs)
    set(CUPOCH_INSTALL_INCLUDE_DIR "${CMAKE_INSTALL_INCLUDEDIR}")
    set(CUPOCH_INSTALL_BIN_DIR "${CMAKE_INSTALL_BINDIR}")
    set(CUPOCH_INSTALL_LIB_DIR "${CMAKE_INSTALL_LIBDIR}")
    set(CUPOCH_INSTALL_CMAKE_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")
else()
    set(CUPOCH_INSTALL_INCLUDE_DIR include)
    set(CUPOCH_INSTALL_BIN_DIR bin)
    set(CUPOCH_INSTALL_LIB_DIR lib)
    set(CUPOCH_INSTALL_CMAKE_DIR CMake)
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

if (WIN32)
    # can't hide the unit testing option on Windows only
    # as a precaution: disable unit testing on Windows regardless of user input
    message(STATUS "Disable unit tests since this feature is not fully supported on Windows.")
    set(BUILD_UNIT_TESTS OFF)
    set(USE_RMM OFF)
    add_definitions(-DWINDOWS)
    add_definitions(-D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE -D_SCL_SECURE_NO_WARNINGS)		# suppress C4996 warning
    add_definitions(-DGLEW_STATIC)		# enable GLEW for windows
    add_definitions(-DTHRUST_CPP11_REQUIRED_NO_ERROR)
    SET(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    message(STATUS "Compiling on Windows")
    if (MSVC)
        message(STATUS "Compiling with MSVC")
        add_definitions(-DNOMINMAX)
        add_definitions(-D_USE_MATH_DEFINES)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc /Zc:__cplusplus /bigobj /std:c++17")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D _ENABLE_EXTENDED_ALIGNED_STORAGE")
        set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MP")
        set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MP")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MP")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MP")
        set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS} --no-host-device-move-forward)
        set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS} -DTHRUST_DEVICE_SYSTEM=THRUST_DEVICE_SYSTEM_CUDA)
    endif (MSVC)

    if (STATIC_WINDOWS_RUNTIME)
        # by default, "/MD" and "/MDd" is set by CMake automatically
        string(REPLACE "/MD" "/MT" CMAKE_C_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE})
        string(REPLACE "/MDd" "/MTd" CMAKE_C_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG})
        string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
        string(REPLACE "/MDd" "/MTd" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
    else ()
        # handles the case when re-running cmake with STATIC_WINDOWS_RUNTIME=OFF
        string(REPLACE "/MT" "/MD" CMAKE_C_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE})
        string(REPLACE "/MTd" "/MDd" CMAKE_C_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG})
        string(REPLACE "/MT" "/MD" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
        string(REPLACE "/MTd" "/MDd" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
    endif ()
    message(STATUS "CMAKE_C_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE}")
    message(STATUS "CMAKE_C_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG}")
    message(STATUS "CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE}")
    message(STATUS "CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG}")
elseif (CYGWIN)
    SET(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    message(STATUS "Compiling on Cygwin")
    # enable c++17
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
    add_definitions(-DCYGWIN)
elseif (APPLE)
    add_definitions(-DUNIX)
    # enable c++17
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
    add_compile_options(-Wno-deprecated-declarations)
    if (NOT BUILD_SHARED_LIBS)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
    endif (NOT BUILD_SHARED_LIBS)
    # In Release build -O3 will be added automatically by CMake
    # We still enable -O3 at Debug build to optimize performance
    if (uppercase_CMAKE_BUILD_TYPE STREQUAL "DEBUG")
        add_definitions(-O3)
    endif()
elseif (UNIX)
    add_definitions(-DUNIX)
    # enable c++17
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -fPIC")
    if (NOT BUILD_SHARED_LIBS)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -fvisibility-inlines-hidden")
    endif (NOT BUILD_SHARED_LIBS)
    add_compile_options(-Wno-deprecated-declarations)
    # In Release build -O3 will be added automatically by CMake
    # We still enable -O3 at debug build to optimize performance
    if (uppercase_CMAKE_BUILD_TYPE STREQUAL "DEBUG")
        add_definitions(-O3)
    endif()
    # disable BUILD_LIBREALSENSE since it is not fully supported on Linux
    message(STATUS "Compiling on Unix")
endif ()

if (USE_HIP)
  # --- ROCm/HIP build ----------------------------------------------------
  # cupoch uses the legacy FindCUDA module (cuda_add_library + CUDA_NVCC_FLAGS).
  # Rather than rewrite every module's CMakeLists, provide a cuda_add_library()
  # shim that creates a normal library and marks the .cu sources LANGUAGE HIP,
  # so each `cuda_add_library(cupoch_xxx ...)` call is untouched and the NVIDIA
  # path stays unchanged. host C++ is compiled by the host compiler; only
  # .cu translation units see the HIP toolchain.
  if(NOT DEFINED CMAKE_HIP_COMPILER AND EXISTS "/opt/rocm/llvm/bin/clang++")
    set(CMAKE_HIP_COMPILER "/opt/rocm/llvm/bin/clang++")
  endif()
  # enable_language(HIP) honors -DCMAKE_HIP_ARCHITECTURES, otherwise auto-detects
  # the host GPU(s), and errors on a no-GPU build host.
  enable_language(HIP)
  # On Windows with clang, the MSVC block above does not fire, so no -std=c++17
  # is set globally. rocPRIM (pulled in by rocThrust) requires C++17; enforce it
  # for all HIP and CXX TUs when the compiler is clang-based on Windows.
  if (WIN32 AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_HIP_STANDARD 17)
    set(CMAKE_HIP_STANDARD_REQUIRED ON)
    # clang-on-Windows does not enter the MSVC block above, so set the two Windows
    # definitions the HIP build needs here (the NVIDIA MSVC build never sees them):
    # NOMINMAX keeps windows.h min/max macros from shadowing Eigen/STL, and
    # _USE_MATH_DEFINES exposes M_PI / M_PI_2 from <cmath>.
    add_definitions(-DNOMINMAX -D_USE_MATH_DEFINES)
  endif ()
  find_package(hip REQUIRED)

  # IMPORTANT: link only hip::host (runtime), never hip::device or roc::rocthrust
  # as a target dependency. hip::device carries `-x hip --offload-arch=...` as
  # INTERFACE_COMPILE_OPTIONS that propagate to ALL languages of a consuming
  # target -- including the host .cpp files in cuda_add_library targets -- which
  # makes g++ choke on `--offload-arch`. The .cu files are marked LANGUAGE HIP and
  # compiled by the HIP toolchain on their own; rocThrust/hipCUB are header-only
  # under /opt/rocm/include (already on the HIP compiler's default search path),
  # so they need no link-time target.
  macro(cuda_add_library _name)
    set(_cupoch_hip_srcs "")
    set(_cupoch_hip_opts "")
    foreach(_arg ${ARGN})
      if(_arg STREQUAL "STATIC" OR _arg STREQUAL "SHARED" OR _arg STREQUAL "MODULE")
        list(APPEND _cupoch_hip_opts ${_arg})
      else()
        list(APPEND _cupoch_hip_srcs ${_arg})
      endif()
    endforeach()
    add_library(${_name} ${_cupoch_hip_opts} ${_cupoch_hip_srcs})
    # Mark every source LANGUAGE HIP, not just .cu. cupoch's GPU library targets
    # mix .cu and .cpp, and several .cpp include rocThrust/stdgpu headers (e.g.
    # utility/helper.cpp via helper.h) that only the HIP (clang) compiler can
    # parse. Under nvcc the whole cuda_add_library was driven by the CUDA
    # compiler, so .cpp there could include CUDA Thrust too; mirror that by
    # compiling all of these TUs with the HIP toolchain (clang compiles plain
    # host C++ unchanged).
    foreach(_src ${_cupoch_hip_srcs})
      if(_src MATCHES "\\.(cu|cpp)$")
        set_source_files_properties(${_src} PROPERTIES LANGUAGE HIP)
      endif()
    endforeach()
    set_target_properties(${_name} PROPERTIES HIP_ARCHITECTURES "${CMAKE_HIP_ARCHITECTURES}")
    target_link_libraries(${_name} hip::host)
  endmacro()

  # CUDA_LIBRARIES/CUDA_INCLUDE_DIRS are referenced by the existing CMake; map
  # them to the HIP runtime so those lines keep working without edits.
  set(CUDA_LIBRARIES hip::host)
  set(CUDA_INCLUDE_DIRS "")
  add_compile_definitions(USE_HIP __HIP_PLATFORM_AMD__)
else ()
find_package(CUDA REQUIRED)
enable_language(CUDA)
if(NOT CMAKE_CUDA_ARCHITECTURES)
  set(CMAKE_CUDA_ARCHITECTURES 86)
endif()
if(NOT CUDA_ARCH_BIN)
  set(_cupoch_cuda_arch_bin "")
  foreach(_cupoch_cuda_arch IN LISTS CMAKE_CUDA_ARCHITECTURES)
    string(REGEX MATCH "^[0-9]+" _cupoch_cuda_arch_num "${_cupoch_cuda_arch}")
    if(_cupoch_cuda_arch_num)
      list(APPEND _cupoch_cuda_arch_bin "${_cupoch_cuda_arch_num}")
    endif()
  endforeach()
  if(_cupoch_cuda_arch_bin)
    set(CUDA_ARCH_BIN "${_cupoch_cuda_arch_bin}")
  endif()
endif()
include(${CMAKE_SOURCE_DIR}/cmake/CudaComputeTargetFlags.cmake)
APPEND_TARGET_ARCH_FLAGS()
list(FIND CUDA_NVCC_FLAGS "-std=c++17" _cupoch_cuda_std_flag_idx)
if(_cupoch_cuda_std_flag_idx EQUAL -1)
  list(APPEND CUDA_NVCC_FLAGS -std=c++17)
endif()
if (NOT cuda_nvcc_target_flags)
  set(_cupoch_cuda_arch_fallback 52)
  foreach(_cupoch_cuda_arch IN LISTS CMAKE_CUDA_ARCHITECTURES)
    string(REGEX MATCH "^[0-9]+" _cupoch_cuda_arch_num "${_cupoch_cuda_arch}")
    if(_cupoch_cuda_arch_num)
      set(_cupoch_cuda_arch_fallback ${_cupoch_cuda_arch_num})
      break()
    endif()
  endforeach()
  list(APPEND CUDA_NVCC_FLAGS -arch=sm_${_cupoch_cuda_arch_fallback})
endif ()
include_directories(${CUDA_INCLUDE_DIRS})
set(CMAKE_CUDA_COMPILE_FEATURES cuda_std_17)
set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS}
    --expt-relaxed-constexpr
    --expt-extended-lambda
    --default-stream per-thread
    --use_fast_math
    -Xcudafe "--diag_suppress=integer_sign_change"
    -Xcudafe "--diag_suppress=partial_override"
    -Xcudafe "--diag_suppress=virtual_function_decl_hidden")
endif ()  # USE_HIP

if (USE_RMM)
    # RMM (RAPIDS Memory Manager) is CUDA-only; the HIP build uses USE_RMM=OFF and
    # falls back to thrust::device_vector (see utility/device_vector.h).
    add_definitions(-DUSE_RMM)
    add_definitions(-DLIBCUDACXX_ENABLE_EXPERIMENTAL_MEMORY_RESOURCE)
    if (NOT USE_HIP)
        set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS} -DLIBCUDACXX_ENABLE_EXPERIMENTAL_MEMORY_RESOURCE)
    endif ()
endif ()

# 3rd-party projects that are added with external_project_add will be installed
# with this prefix. E.g.
# - 3RDPARTY_INSTALL_PREFIX: cupoch/build/3rdparty_install
# - Headers: cupoch/build/3rdparty_install/include/extern_lib.h
# - Libraries: cupoch/build/3rdparty_install/lib/extern_lib.a
set(3RDPARTY_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/3rdparty_install")

if (USE_HIP)
    # Full ROCm/HIP build: the same module set as the NVIDIA build, minus only
    # the bits that are genuinely non-viable on ROCm (decided automatically, not
    # behind a user flag). The third-party graph is assembled by a HIP-specific
    # file rather than third_party/CMakeLists.txt (which bakes in the legacy
    # FindCUDA assumptions); see cmake/cupoch_hip_3rdparty.cmake.
    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/cupoch_hip_modules.cmake)
else ()
    add_subdirectory(third_party)

    include_directories(
        SYSTEM
        ${3RDPARTY_INCLUDE_DIRS}
    )

    include_directories(src)
    add_subdirectory(src)
    add_subdirectory(examples)
endif ()
