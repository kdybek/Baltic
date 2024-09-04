cmake_minimum_required(VERSION 3.28)
project(Baltic LANGUAGES CXX)

if(NOT WIN32)
    message(FATAL_ERROR "Baltic can only be built on Windows.")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Configurations" FORCE)

include_directories(${CMAKE_SOURCE_DIR}/src)

set(SOURCE_DIR src)

set(SOURCES
    ${SOURCE_DIR}/main.cpp
    ${SOURCE_DIR}/d3d/dx_context.cpp
    ${SOURCE_DIR}/d3d/dx_context.h
    ${SOURCE_DIR}/auxiliary/win_include.h
    ${SOURCE_DIR}/auxiliary/baltic_except.h
    ${SOURCE_DIR}/auxiliary/constants.h
    ${SOURCE_DIR}/auxiliary/dx_window.cpp
    ${SOURCE_DIR}/auxiliary/dx_window.h
    ${SOURCE_DIR}/auxiliary/shader.cpp
    ${SOURCE_DIR}/auxiliary/shader.h
    ${SOURCE_DIR}/debug/dx_debug_layer.h
)

set(SHADER_DIR src/shaders)

set(SHADERS
    ${SHADER_DIR}/vertex_shader.hlsl
    ${SHADER_DIR}/pixel_shader.hlsl
)

set_source_files_properties(${SHADER_DIR}/vertex_shader.hlsl PROPERTIES SHADER_TYPE "vs")
set_source_files_properties(${SHADER_DIR}/pixel_shader.hlsl PROPERTIES SHADER_TYPE "ps")
set_source_files_properties(${SHADERS} PROPERTIES SHADER_MODEL "6_6")

foreach(SHADER ${SHADERS})
    get_filename_component(SHADER_NAME ${SHADER} NAME_WE)
    get_source_file_property(SHADER_TYPE ${SHADER} SHADER_TYPE)
    get_source_file_property(SHADER_MODEL ${SHADER} SHADER_MODEL)
    set(COMPILED_SHADER ${SHADER_NAME}.cso)

    add_custom_command(
        OUTPUT ${COMPILED_SHADER}
        COMMAND dxc -T ${SHADER_TYPE}_${SHADER_MODEL} -E main -Fo $<TARGET_FILE_DIR:baltic>/${COMPILED_SHADER} ${CMAKE_SOURCE_DIR}/${SHADER}
        DEPENDS ${SHADER}
        COMMENT "Compiling shader: ${SHADER_NAME}"
    )

    list(APPEND COMPILED_SHADERS ${COMPILED_SHADER})
endforeach()

add_custom_target(shaders ALL DEPENDS ${COMPILED_SHADERS})

add_executable(baltic ${SOURCES})

add_dependencies(baltic shaders)

target_link_libraries(baltic
    d3d12.lib
    dxgi.lib
    dxguid.lib
    runtimeobject.lib
)

target_compile_definitions(baltic PRIVATE $<$<CONFIG:Debug>:BALTIC_DEBUG>)
