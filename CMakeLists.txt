cmake_minimum_required(VERSION 3.20)
project(cppixelsorter)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Option for static/dynamic linking
option(BUILD_STATIC "Build with static libraries" OFF)

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp -Wall -Wno-unused-parameter -march=native")

# Debug and Release flags
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0 -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Default to Debug if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Include directories
include_directories(hdr)

# Configure SDL3 and SDL3_ttf based on static/dynamic preference
if(BUILD_STATIC)
    set(CMAKE_FIND_LIBRARY_SUFFICES ".a")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++")
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)
else()
    set(BUILD_SHARED_LIBS ON CACHE BOOL "Build shared libraries" FORCE)
endif()

# Add SDL3 and SDL3_ttf as subdirectories
add_subdirectory(dependencies/SDL)
add_subdirectory(dependencies/SDL_ttf)

# Create executable
add_executable(cppixelsorter ${SOURCES})

# Link libraries
target_link_libraries(cppixelsorter PRIVATE SDL3::SDL3 SDL3_ttf::SDL3_ttf)

# Link OpenMP
find_package(OpenMP REQUIRED)
target_link_libraries(cppixelsorter PRIVATE OpenMP::OpenMP_CXX)

# Embed data (custom command to generate embed_data.cpp)
find_program(XXD_PROGRAM xxd)
find_program(PYTHON_PROGRAM python3 python)

if(XXD_PROGRAM AND PYTHON_PROGRAM)
    add_custom_target(embeds
        COMMAND ${XXD_PROGRAM} -n assets_icon_png -i ${CMAKE_SOURCE_DIR}/assets/icon.png > ${CMAKE_SOURCE_DIR}/src/embed_data.cpp
        COMMAND ${XXD_PROGRAM} -n assets_Archivo_Regular_ttf -i ${CMAKE_SOURCE_DIR}/assets/Archivo-Regular.ttf >> ${CMAKE_SOURCE_DIR}/src/embed_data.cpp
        COMMAND ${PYTHON_PROGRAM} ${CMAKE_SOURCE_DIR}/scripts/huebar.py | ${XXD_PROGRAM} -i -n assets_huebar >> ${CMAKE_SOURCE_DIR}/src/embed_data.cpp
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
    add_dependencies(cppixelsorter embeds)
endif()

# Strip release build
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_custom_command(TARGET cppixelsorter POST_BUILD
        COMMAND ${CMAKE_STRIP} $<TARGET_FILE:cppixelsorter>
    )
endif()

# Install target
install(TARGETS cppixelsorter DESTINATION ~/.local/bin/)

