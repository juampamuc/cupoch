# Copyright (c) 2026 Advanced Micro Devices, Inc.
# ROCm/HIP port author: Jeff Daily <jeff.daily@amd.com>
#
# Third-party dependency assembly for the full ROCm/HIP build. The HIP build
# does NOT go through third_party/CMakeLists.txt: that file builds the GPU deps
# (stdgpu, flann CUDA kdtree, libSGM) with the legacy FindCUDA assumptions and
# would need every one made HIP-aware in-line. Instead this file builds the
# same dependency set the HIP module graph needs, with the stdgpu/flann HIP
# bring-up already proven by the core path plus the host libraries the io /
# kinematics / visualization modules require. The NVIDIA build is unaffected
# (it never includes this file).

set(_tp ${CMAKE_CURRENT_SOURCE_DIR}/third_party)

# --- header-only / host deps ------------------------------------------------
set(EIGEN3_INCLUDE_DIRS ${_tp}/eigen)
set(dlpack_INCLUDE_DIRS ${_tp}/dlpack/include)
set(tomasakeninemoeller_INCLUDE_DIRS ${_tp}/tomasakeninemoeller)
set(flann_INCLUDE_DIRS ${_tp})
set(lbvh_INCLUDE_DIRS ${_tp}/lbvh)
set(lbvh_index_INCLUDE_DIRS ${_tp}/lbvh_index)

# spdlog (built as the project does, header set exported)
set(SPDLOG_MASTER_PROJECT OFF)
add_subdirectory(${_tp}/spdlog ${CMAKE_BINARY_DIR}/spdlog)
set(spdlog_INCLUDE_DIRS ${_tp}/spdlog/include)

# jsoncpp (utility/ijson_convertible + camera + io)
add_subdirectory(${_tp}/jsoncpp ${CMAKE_BINARY_DIR}/jsoncpp)
set(JSONCPP_INCLUDE_DIRS ${_tp}/jsoncpp/include)
set(JSONCPP_LIBRARIES jsoncpp)

# liblzf (io: compressed voxel/feature blobs)
file(GLOB LIBLZF_SOURCE_FILES "${_tp}/liblzf/*.c")
add_library(liblzf ${LIBLZF_SOURCE_FILES})
target_include_directories(liblzf PUBLIC ${_tp}/liblzf)
set(liblzf_INCLUDE_DIRS ${_tp}/liblzf)
set(liblzf_LIBRARIES liblzf)

# rply (io: PLY point clouds / meshes)
add_library(rply ${_tp}/rply/rply.c)
set(rply_INCLUDE_DIRS ${_tp}/rply)
set(rply_LIBRARIES rply)

# tinyobjloader (io: OBJ meshes)
add_library(tinyobjloader STATIC ${_tp}/tinyobjloader/tiny_obj_loader.cc)
target_include_directories(tinyobjloader PUBLIC ${_tp}/tinyobjloader)
set(tinyobjloader_INCLUDE_DIRS ${_tp}/tinyobjloader)
set(tinyobjloader_LIBRARIES tinyobjloader)

# zlib + libpng (io: PNG images). Build from the vendored sources (as the
# project's own third_party/CMakeLists.txt does) so the build does not depend on
# a system libpng matching the vendored headers. The vendored zlib/libpng define
# the targets `zlib` and `png` (libpng's CMakeLists reads ZLIB_LIBRARY/_INCLUDE
# from the parent scope to find the just-built zlib).
add_subdirectory(${_tp}/zlib ${CMAKE_BINARY_DIR}/zlib)
set(ZLIB_INCLUDE_DIR ${_tp}/zlib ${CMAKE_BINARY_DIR}/zlib)
set(ZLIB_LIBRARY zlib)
add_subdirectory(${_tp}/libpng ${CMAKE_BINARY_DIR}/libpng)
set(PNG_INCLUDE_DIRS ${_tp}/libpng ${CMAKE_BINARY_DIR}/libpng)
set(PNG_LIBRARIES png zlib)

# libjpeg-turbo (io: classic libjpeg API via <jpeglib.h>). Build the vendored
# source in-tree (add_subdirectory) rather than as an ExternalProject: the
# project's libjpeg-turbo.cmake hard-codes a source path relative to
# third_party/, and the static jpeg target plus its generated jconfig/jpeglib
# headers are all we need here.
set(ENABLE_SHARED OFF CACHE BOOL "" FORCE)
set(ENABLE_STATIC ON CACHE BOOL "" FORCE)
set(WITH_TURBOJPEG OFF CACHE BOOL "" FORCE)
# NASM is not a build dependency here; disable the SIMD path so libjpeg-turbo
# does not enable_language(ASM_NASM) and fail when nasm is absent.
set(WITH_SIMD OFF CACHE BOOL "" FORCE)
set(REQUIRE_SIMD OFF CACHE BOOL "" FORCE)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
# libjpeg-turbo's CMakeLists.txt uses CMAKE_INSTALL_DOCDIR unconditionally.
# On non-UNIX platforms CMake's GNUInstallDirs may leave it empty; set a
# fallback so libjpeg-turbo's install() calls don't fail at configure time.
if(NOT CMAKE_INSTALL_DOCDIR)
    set(CMAKE_INSTALL_DOCDIR "doc" CACHE PATH "" FORCE)
endif()
add_subdirectory(${_tp}/libjpeg-turbo/libjpeg-turbo ${CMAKE_BINARY_DIR}/libjpeg-turbo)
set(JPEG_TURBO_INCLUDE_DIRS ${_tp}/libjpeg-turbo/libjpeg-turbo
    ${CMAKE_BINARY_DIR}/libjpeg-turbo)
set(JPEG_TURBO_LIBRARIES jpeg-static)

# urdfdom (kinematics: URDF robot model parser, host C++)
set(CONSOLE_BRIDGE_MAJOR_VERSION 1)
set(CONSOLE_BRIDGE_MINOR_VERSION 0)
set(CONSOLE_BRIDGE_PATCH_VERSION 1)
include(GenerateExportHeader)
add_library(console_bridge ${_tp}/urdfdom/urdf_parser/src/console.cpp)
target_include_directories(console_bridge PUBLIC
    $<BUILD_INTERFACE:${_tp}/urdfdom/urdf_parser/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)
generate_export_header(console_bridge EXPORT_MACRO_NAME CONSOLE_BRIDGE_DLLAPI)
file(GLOB_RECURSE URDFDOM_SOURCES ${_tp}/urdfdom/urdf_parser/src/*.cpp)
list(FILTER URDFDOM_SOURCES EXCLUDE REGEX "src/console\\.cpp$")
add_library(urdfdom STATIC ${URDFDOM_SOURCES})
target_include_directories(urdfdom PRIVATE
    ${_tp}/urdfdom/urdf_parser/include
    ${_tp}/urdfdom/urdf_parser/include/tinyxml)
target_link_libraries(urdfdom console_bridge)
set(urdfdom_INCLUDE_DIR
    ${_tp}/urdfdom/urdf_parser/include
    ${_tp}/urdfdom/urdf_parser/include/tinyxml
    ${CMAKE_CURRENT_BINARY_DIR})
set(urdfdom_LIBRARIES urdfdom)

# --- stdgpu (GPU STL, HIP backend) ------------------------------------------
set(STDGPU_BUILD_EXAMPLES OFF CACHE INTERNAL "")
set(STDGPU_BUILD_TESTS OFF CACHE INTERNAL "")
set(STDGPU_BUILD_BENCHMARKS OFF CACHE INTERNAL "")
set(STDGPU_SETUP_COMPILER_FLAGS OFF CACHE INTERNAL "")
set(STDGPU_BACKEND STDGPU_BACKEND_HIP CACHE STRING "" FORCE)
# stdgpu 1.3.0 bitrots against ROCm 7: its bundled Findthrust requires CUDA-only
# CUB/libcudacxx, and it pins find_package(hip 5.1) which the SameMajorVersion
# ROCm 7 hip-config rejects. Resolve both from cupoch's side (no submodule edit).
list(PREPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/hip_thrust)
list(PREPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/rocm_hip_compat)
add_subdirectory(${_tp}/stdgpu ${CMAKE_BINARY_DIR}/stdgpu)
# stdgpu compiles its generic impl/*.cpp with the CXX compiler, but on the HIP
# backend those include rocThrust -> rocPRIM headers only the HIP (clang)
# compiler can parse; force every stdgpu TU to LANGUAGE HIP (absolute paths).
get_target_property(_stdgpu_srcs stdgpu SOURCES)
get_target_property(_stdgpu_srcdir stdgpu SOURCE_DIR)
set(_stdgpu_hip_srcs "")
foreach(_s ${_stdgpu_srcs})
    if(_s MATCHES "\\.(cpp|cu)$")
        if(NOT IS_ABSOLUTE "${_s}")
            set(_s "${_stdgpu_srcdir}/${_s}")
        endif()
        list(APPEND _stdgpu_hip_srcs "${_s}")
    endif()
endforeach()
set_source_files_properties(${_stdgpu_hip_srcs} TARGET_DIRECTORY stdgpu
                            PROPERTIES LANGUAGE HIP)
if (NOT WIN32)
    target_compile_options(stdgpu PRIVATE $<$<COMPILE_LANGUAGE:HIP>:-fPIC>)
endif ()
set(stdgpu_INCLUDE_DIRS ${_tp}/stdgpu/src ${CMAKE_BINARY_DIR}/stdgpu/src/stdgpu/include)
set(stdgpu_LIBRARIES stdgpu)

# --- OpenGL renderer deps (visualization; attempted when OpenGL is present) --
# ROCm ships the hipGraphics* GL<->HIP interop API, so the GL renderer is a real
# target on AMD. It needs an OpenGL context (GLFW), the GL loader (GLEW) and the
# imgui overlay. GLFW + OpenGL come from the system; GLEW + imgui build from the
# vendored sources. If OpenGL is absent the visualization module is skipped
# automatically (decided in cupoch_hip_modules.cmake).
find_package(OpenGL QUIET)
set(CUPOCH_HIP_BUILD_VISUALIZATION OFF)
find_package(glfw3 QUIET)
if (OPENGL_FOUND)
    set(CUPOCH_HIP_BUILD_VISUALIZATION ON)

    add_subdirectory(${_tp}/glew ${CMAKE_BINARY_DIR}/glew)
    set(GLEW_INCLUDE_DIRS ${_tp}/glew/include)
    set(GLEW_LIBRARIES glew)

    # Prefer a system GLFW; fall back to the vendored copy.
    if (glfw3_FOUND OR TARGET glfw)
        set(GLFW_LIBRARIES glfw)
        set(GLFW_INCLUDE_DIRS ${OPENGL_INCLUDE_DIR})
    else ()
        set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "")
        set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "")
        set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "")
        add_subdirectory(${_tp}/GLFW ${CMAKE_BINARY_DIR}/GLFW)
        set(GLFW_LIBRARIES glfw)
        # The vendored GLFW headers are in third_party/GLFW/include; also
        # include OPENGL_INCLUDE_DIR (may be empty on Windows but harmless).
        set(GLFW_INCLUDE_DIRS ${_tp}/GLFW/include ${OPENGL_INCLUDE_DIR})
    endif ()

    add_library(imgui STATIC
        ${_tp}/imgui/imgui.cpp
        ${_tp}/imgui/imgui_draw.cpp
        ${_tp}/imgui/imgui_tables.cpp
        ${_tp}/imgui/imgui_widgets.cpp
        ${_tp}/imgui/backends/imgui_impl_glfw.cpp
        ${_tp}/imgui/backends/imgui_impl_opengl3.cpp)
    target_include_directories(imgui PRIVATE ${_tp}/imgui ${GLFW_INCLUDE_DIRS}
                               ${GLEW_INCLUDE_DIRS})
    target_include_directories(imgui PUBLIC ${_tp})
    set(imgui_INCLUDE_DIRS ${_tp} ${_tp}/imgui)
    set(imgui_LIBRARIES imgui)
endif ()

# --- flann CUDA kdtree (knn / geometry normals) -----------------------------
file(GLOB_RECURSE CUPOCH_FLANN_CU ${_tp}/flann/*.cu)
cuda_add_library(flann_cuda_s STATIC ${CUPOCH_FLANN_CU})
target_include_directories(flann_cuda_s BEFORE PRIVATE
                           ${CMAKE_CURRENT_SOURCE_DIR}/cmake/flann_hip_compat)
target_compile_options(flann_cuda_s PRIVATE
                       -include ${CMAKE_CURRENT_SOURCE_DIR}/cmake/flann_hip_compat/flann_hip_prelude.h)
target_include_directories(flann_cuda_s PRIVATE ${flann_INCLUDE_DIRS} ${spdlog_INCLUDE_DIRS})
target_compile_definitions(flann_cuda_s PRIVATE -DFLANN_USE_CUDA)
set(FLANN_LIBRARIES flann_cuda_s)

# --- exported dependency lists ----------------------------------------------
set(3RDPARTY_INCLUDE_DIRS
    ${EIGEN3_INCLUDE_DIRS}
    ${flann_INCLUDE_DIRS}
    ${spdlog_INCLUDE_DIRS}
    ${dlpack_INCLUDE_DIRS}
    ${JSONCPP_INCLUDE_DIRS}
    ${stdgpu_INCLUDE_DIRS}
    ${lbvh_INCLUDE_DIRS}
    ${lbvh_index_INCLUDE_DIRS}
    ${tomasakeninemoeller_INCLUDE_DIRS}
    ${liblzf_INCLUDE_DIRS}
    ${rply_INCLUDE_DIRS}
    ${tinyobjloader_INCLUDE_DIRS}
    ${PNG_INCLUDE_DIRS}
    ${JPEG_TURBO_INCLUDE_DIRS}
    ${urdfdom_INCLUDE_DIR}
    ${GLEW_INCLUDE_DIRS}
    ${GLFW_INCLUDE_DIRS}
    ${imgui_INCLUDE_DIRS}
)
set(3RDPARTY_LIBRARIES
    ${FLANN_LIBRARIES}
    ${stdgpu_LIBRARIES}
    ${JSONCPP_LIBRARIES}
    ${liblzf_LIBRARIES}
    ${rply_LIBRARIES}
    ${tinyobjloader_LIBRARIES}
    ${PNG_LIBRARIES}
    ${JPEG_TURBO_LIBRARIES}
    ${urdfdom_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${GLFW_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${imgui_LIBRARIES}
    ${CUDA_LIBRARIES}
)
