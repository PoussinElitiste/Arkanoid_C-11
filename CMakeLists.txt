# Sample Version Of CMake minimum setup
cmake_minimum_required(VERSION 3.15)

#Project name and version
set(CMAKE_GENERATOR_PLATFORM Win32) # x64 or Win32
project(Arkanoid VERSION 0.1 LANGUAGES CXX)

#set variable 
set(ARKANOID_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}")
set(EXECUTABLE_OUTPUT_PATH ${ARKANOID_ROOT_DIR}/Bin)

message(STATUS "ARKANOID_ROOT_DIR: ${ARKANOID_ROOT_DIR}")

#Sub project:
#- directory must exist with a CMakeLists inside
add_subdirectory(Source/Core)
#add_subdirectory(Source)

# Compiler setup
if (MSVC)
    add_compile_options(/W4)
else()
    # Clang all warning
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# configure header file to pass version number
configure_file("${ARKANOID_ROOT_DIR}/Source/Game/ArkanoidConfig.h.in" "${ARKANOID_ROOT_DIR}/Source/Game/ArkanoidConfig.h")

# external libraries
# SFML
set(SFML_STATIC_LIBRARIES TRUE)

if (CMAKE_GENERATOR_PLATFORM MATCHES Win32)
    set(SFML_DIR ${ARKANOID_ROOT_DIR}/Library/SFML-2.5.1/x32/lib/cmake/SFML)
endif()

if (CMAKE_GENERATOR_PLATFORM MATCHES x64)
    set(SFML_DIR ${ARKANOID_ROOT_DIR}/Library/SFML-2.5.1/x64/lib/cmake/SFML)
endif()

set(CORE_DIR ${ARKANOID_ROOT_DIR}/Source/Core)
find_package(SFML 2 COMPONENTS graphics audio REQUIRED)

# add source files
#file( GLOB SRCS 
#    ${ARKANOID_ROOT_DIR}/Source/*.c 
#    ${ARKANOID_ROOT_DIR}/Source/*.cpp 
#    ${ARKANOID_ROOT_DIR}/Source/*.h 
#    ${ARKANOID_ROOT_DIR}/Source/*.hpp 
#    ${ARKANOID_ROOT_DIR}/Source/Core/*.c 
#    ${ARKANOID_ROOT_DIR}/Source/Core/*.cpp 
#    ${ARKANOID_ROOT_DIR}/Source/Core/*.h 
#    ${ARKANOID_ROOT_DIR}/Source/Core/*.hpp 
#    )

# add executable
#add_executable( ${PROJECT_NAME} ${SRCS} )

file(GLOB HEADER_FILES_CLIENT "${ARKANOID_ROOT_DIR}/Source/Game/*.h")
file(GLOB SOURCE_FILES_CLIENT "${ARKANOID_ROOT_DIR}/Source/Game/*.cpp")

# Setup your library or executable:
add_executable(Arkanoid ${HEADER_FILES_CLIENT} ${SOURCE_FILES_CLIENT})
include_directories(${ARKANOID_ROOT_DIR}/Source/Core) # include files from Core

# Here's the important part ("Header Files" and "Source Files" are literals.)
#source_group("Header\\core" ${HEADER_FILES_CLIENT_CORE})
#source_group("Source\\core" ${SOURCE_FILES_CLIENT_CORE})

#file(GLOB_RECURSE _source_list *.cpp* *.h* *.hpp*)
#foreach(_source IN ITEMS ${_source_list})
#    get_filename_component(_source_path "${_source}" PATH)
#    string(REPLACE "${CMAKE_SOURCE_DIR}" "" _group_path "${_source_path}")
#    string(REPLACE "/" "\\" _group_path "${_group_path}")
#    source_group("${_group_path}" FILES "${_source}")
#endforeach()

# setup project properties
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Arkanoid)
set_target_properties(Core PROPERTIES LINKER_LANGUAGE CXX)

# link include target
target_link_libraries(Arkanoid sfml-graphics sfml-audio Core)

# add the binary tree to the search path for include files
# so that we will find ArkanoidConfig.h
target_include_directories(Arkanoid PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           )

# add the install targets
install(TARGETS Arkanoid DESTINATION bin)
install(FILES "${PROJECT_BINARY_DIR}/ArkanoidConfig.h"
  DESTINATION include
  )