# Include the directory itself as a path to include directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CORE_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}")

# get Header files recursive
FILE(GLOB_RECURSE header_files_core "${CORE_ROOT_DIR}/*.h")
LIST(REMOVE_DUPLICATES header_files_core)

list(LENGTH header_files_core header_files_count)
message(STATUS "[INFO] Found ${header_files_count} header files.")
# file(GLOB HEADER_FILES_CORE "${CORE_ROOT_DIR}/*.h")
function(assign_source_group filter_name)
    foreach(_source IN ITEMS ${ARGN})
        if (IS_ABSOLUTE "${_source}")
            file(RELATIVE_PATH _source_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source}")
            message(STATUS "[INFO] ${_source_rel}")
        else()
            set(_source_rel "${_source}")
            message(STATUS "[INFO] ${_source_rel}")
        endif()
        get_filename_component(_source_path "${_source_rel}" PATH)
        string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
        source_group("Header\\${_source_path_msvc}" FILES "${_source}")
    endforeach()
endfunction(assign_source_group)

assign_source_group(${header_files_core})

file(GLOB SOURCE_FILES_CORE "${CORE_ROOT_DIR}/*.cpp")

# Setup your library or executable:
add_library(Core ${header_files_core} ${SOURCE_FILES_CORE})

install(TARGETS Core
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)