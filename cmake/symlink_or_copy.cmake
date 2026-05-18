# cmake/symlink_or_copy.cmake
#
# Attempts to create a symlink from LINK_PATH -> SOURCE_PATH.
# Falls back to copy_directory_if_different if symlink creation fails
# (e.g. on Windows without Developer Mode or admin privileges).
#
# Called via:
#   cmake -DSOURCE_PATH=... -DLINK_PATH=... -P cmake/symlink_or_copy.cmake

if(NOT DEFINED SOURCE_PATH OR NOT DEFINED LINK_PATH)
    message(FATAL_ERROR "symlink_or_copy.cmake requires SOURCE_PATH and LINK_PATH to be defined.")
endif()

# Ensure the destination's parent directory exists before attempting anything.
cmake_path(GET LINK_PATH PARENT_PATH LINK_PARENT)
file(MAKE_DIRECTORY "${LINK_PARENT}")

# If the symlink/directory already exists and is correct, nothing to do.
if(IS_SYMLINK "${LINK_PATH}")
    message(STATUS "Assets symlink already exists, skipping.")
    return()
endif()

# Attempt symlink first.
message(STATUS "Symlinking: ${LINK_PATH} -> ${SOURCE_PATH}")
execute_process(
    COMMAND ${CMAKE_COMMAND} -E create_symlink "${SOURCE_PATH}" "${LINK_PATH}"
    RESULT_VARIABLE SYMLINK_RESULT
    ERROR_VARIABLE  SYMLINK_ERROR
)

if(SYMLINK_RESULT EQUAL 0)
    message(STATUS "Symlink created successfully.")
    return()
endif()

# Symlink failed — fall back to copying.
message(WARNING
    "Symlink creation failed: ${SYMLINK_ERROR}\n"
    "Falling back to copy_directory_if_different.\n"
    "To enable symlinks on Windows, turn on Developer Mode:\n"
    "  Settings -> System -> For Developers -> Developer Mode"
)

message(STATUS "Copying: ${SOURCE_PATH} -> ${LINK_PATH}")
execute_process(
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different "${SOURCE_PATH}" "${LINK_PATH}"
    RESULT_VARIABLE COPY_RESULT
    ERROR_VARIABLE  COPY_ERROR
)

if(NOT COPY_RESULT EQUAL 0)
    message(FATAL_ERROR "Fallback copy also failed: ${COPY_ERROR}")
endif()

message(STATUS "Assets copied successfully.")