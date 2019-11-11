# Sample Version Of CMake minimum setup
cmake_minimum_required(VERSION 3.15)

#Project name and version
project(Arkanoid VERSION 0.1)

#include_directories(${PROJECT_BINARY_BIN})
set(EXECUTABLE_OUTPUT_PATH "../bin")

if (MSVC)
    add_compile_options(/W4)
else()
    # Clang all warning
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# external libraries
set(SFML_STATIC_LIBRARIES TRUE)
set(SFML_DIR "../SFML-2.5.1/x32/lib/cmake/SFML")
find_package(SFML 2 COMPONENTS graphics audio REQUIRED)

# configure header file to pass version number
configure_file(ArkanoidConfig.h.in ArkanoidConfig.h)

# add executable
add_executable(Arkanoid Arkanoid.cpp)
target_link_libraries(Arkanoid sfml-graphics sfml-audio)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Arkanoid)

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