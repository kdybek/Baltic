cmake_minimum_required(VERSION 3.28)
project(Baltic LANGUAGES CXX)

if(NOT WIN32)
    message(FATAL_ERROR "Baltic can only be built on Windows.")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SOURCES
        src/main.cpp
        src/auxiliary/win_include.h
        src/baltic_except.h
        src/debug/dx_debug_layer.h
        src/d3d/dx_context.cpp
        src/d3d/dx_context.h
)

add_executable(baltic ${SOURCES})

target_link_libraries(baltic
        d3d12.lib
        dxgi.lib
        dxguid.lib
        runtimeobject.lib
)

target_compile_definitions(baltic PRIVATE $<$<CONFIG:Debug>:BALTIC_DEBUG>)
