# Include the directory itself as a path to include directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CORE_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}")

file(GLOB HEADER_FILES_CORE "${CORE_ROOT_DIR}/*.h")
file(GLOB SOURCE_FILES_CORE "${CORE_ROOT_DIR}/*.cpp")

# Setup your library or executable:
add_library(Core ${HEADER_FILES_CORE} ${SOURCE_FILES_CORE})

install(TARGETS Core
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)