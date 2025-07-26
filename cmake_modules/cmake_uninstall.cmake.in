# Check if install manifest file exists
if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/install_manifest.txt")
  message(FATAL_ERROR "Cannot find install manifest: \"${CMAKE_CURRENT_BINARY_DIR}/install_manifest.txt\"")
endif()

# Read install manifest file
file(READ "${CMAKE_CURRENT_BINARY_DIR}/install_manifest.txt" files)

# Replace newline with semicolons
string(REGEX REPLACE "\n" ";" files "${files}")

# Iterate over  list:
foreach(file IN LISTS files)
  message(STATUS "Uninstalling \"${ENV{DESTDIR}}${file}\"")
  # Check if file exists
  if(EXISTS "${ENV{DESTDIR}}${file}")
    # Remove file
    execute_process(
      COMMAND "${CMAKE_COMMAND}" -E remove "${ENV{DESTDIR}}${file}"
      OUTPUT_VARIABLE rm_out
      RESULT_VARIABLE rm_retval
    )
    # Check if the removal was successful
    if(NOT "${rm_retval}" STREQUAL "0")
      message(FATAL_ERROR "Problem when removing \"${ENV{DESTDIR}}${file}\"")
    endif()
  else()
    # File does not exist
    message(STATUS "File \"${ENV{DESTDIR}}${file}\" does not exist.")
  endif()
endforeach()
