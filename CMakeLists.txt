# Sample Version Of CMake minimum setup
cmake_minimum_required(VERSION 3.15)

#Project name and version
project(Arkanoid VERSION 0.1)

set(ARKANOID_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}")
set(EXECUTABLE_OUTPUT_PATH ${ARKANOID_ROOT_DIR}/Bin)

# Compiler setup
if (MSVC)
    add_compile_options(/W4)
else()
    # Clang all warning
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# external libraries
set(SFML_STATIC_LIBRARIES TRUE)
set(SFML_DIR ${ARKANOID_ROOT_DIR}/SFML-2.5.1/x32/lib/cmake/SFML)
find_package(SFML 2 COMPONENTS graphics audio REQUIRED)

# configure header file to pass version number
configure_file("${ARKANOID_ROOT_DIR}/Source/ArkanoidConfig.h.in" "${ARKANOID_ROOT_DIR}/Source/ArkanoidConfig.h")

# add executable
file( GLOB SRCS ${ARKANOID_ROOT_DIR}/Source/*.c ${ARKANOID_ROOT_DIR}/Source/*.cpp ${ARKANOID_ROOT_DIR}/Source/*.h ${ARKANOID_ROOT_DIR}/Source/*.hpp )
#set_property(GLOBAL PROPERTY USE_FOLDERS ON)
#add_executable(Arkanoid Arkanoid.cpp)
add_executable( ${PROJECT_NAME} ${SRCS} )
target_link_libraries(Arkanoid sfml-graphics sfml-audio)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Arkanoid)

# add source files
#globrecurse_c_cpp_sources(DUAL_SOURCES ${ARKANOID_ROOT_DIR}/Source)
#group_sources(${ARKANOID_ROOT_DIR}/Source PREFIX "game-cpp" ${ARKANOID_ROOT_DIR})

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