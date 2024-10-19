cmake_minimum_required(VERSION 3.28)
project(Baltic LANGUAGES CXX)

if(NOT WIN32)
    message(FATAL_ERROR "Baltic can only be built on Windows.")
endif()

option(UNICODE "Use Unicode" ON)

set(CMAKE_WIN32_EXECUTABLE ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Configurations" FORCE)

set(SOURCE_DIR "src")
set(SHADER_DIR "${SOURCE_DIR}/shaders")
set(VENDOR_DIR "vendor")
set(IMGUI_DIR "${VENDOR_DIR}/imgui")

file(GLOB_RECURSE SOURCES
    "${SOURCE_DIR}/*.cpp"
    "${SOURCE_DIR}/*.hpp"
)

set(VERTEX_SHADERS "${SHADER_DIR}/vertex_shader.hlsl")
set(PIXEL_SHADERS "${SHADER_DIR}/pixel_shader.hlsl")
set(ROOT_SIGNATURES "${SHADER_DIR}/root_signature.hlsli")

set(SHADERS
    ${VERTEX_SHADERS}
    ${GEOMETRY_SHADERS}
    ${PIXEL_SHADERS}
    ${ROOT_SIGNATURES}
)

set_source_files_properties(${VERTEX_SHADERS} PROPERTIES SHADER_TYPE "vs")
set_source_files_properties(${PIXEL_SHADERS} PROPERTIES SHADER_TYPE "ps")
set_source_files_properties(${ROOT_SIGNATURES} PROPERTIES SHADER_TYPE "rootsig")

set_source_files_properties(${VERTEX_SHADERS} ${PIXEL_SHADERS} PROPERTIES SHADER_MODEL "6_5")
set_source_files_properties(${ROOT_SIGNATURES} PROPERTIES SHADER_MODEL "1_1")

set_source_files_properties(${VERTEX_SHADERS} ${PIXEL_SHADERS} PROPERTIES SHADER_ENTRY "main")
set_source_files_properties(${ROOT_SIGNATURES} PROPERTIES SHADER_ENTRY "ROOTSIG")

foreach(SHADER ${SHADERS})
    get_filename_component(SHADER_NAME ${SHADER} NAME_WE)
    get_source_file_property(SHADER_TYPE ${SHADER} SHADER_TYPE)
    get_source_file_property(SHADER_MODEL ${SHADER} SHADER_MODEL)
    get_source_file_property(SHADER_ENTRY ${SHADER} SHADER_ENTRY)
    set(COMPILED_SHADER ${SHADER_NAME}.cso)

    add_custom_command(
        OUTPUT ${COMPILED_SHADER}
        COMMAND dxc -T ${SHADER_TYPE}_${SHADER_MODEL} -E ${SHADER_ENTRY} -O2 -Fo $<TARGET_FILE_DIR:baltic>/${COMPILED_SHADER} ${CMAKE_SOURCE_DIR}/${SHADER}
        DEPENDS ${SHADER}
        COMMENT "Compiling shader ${CMAKE_SOURCE_DIR}/${SHADER}"
    )

    list(APPEND COMPILED_SHADERS ${COMPILED_SHADER})
endforeach()

add_custom_target(shaders ALL DEPENDS ${COMPILED_SHADERS})

file(GLOB IMGUI_SOURCES 
    "${IMGUI_DIR}/*.cpp" 
    "${IMGUI_DIR}/*.h"
    "${IMGUI_DIR}/backends/imgui_impl_dx12.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_dx12.h"
    "${IMGUI_DIR}/backends/imgui_impl_win32.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_win32.h"
)

add_executable(baltic ${SOURCES} ${IMGUI_SOURCES})

add_dependencies(baltic shaders)

target_link_libraries(baltic
    d3d12.lib
    dxgi.lib
    dxguid.lib
    runtimeobject.lib
)

target_include_directories(baltic PRIVATE ${SOURCE_DIR} ${IMGUI_DIR})

target_precompile_headers(baltic PRIVATE ${SOURCE_DIR}/auxiliary/pch.hpp)

target_compile_definitions(baltic PRIVATE $<$<CONFIG:Debug>:BALTIC_DEBUG>)

if(UNICODE)
    target_compile_definitions(baltic PRIVATE UNICODE _UNICODE)
    message(STATUS "Unicode is enabled")
else()
    message(STATUS "Unicode is disabled")
endif()
