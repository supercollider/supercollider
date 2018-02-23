include_directories(
  ${PROJECT_SOURCE_DIR}/dynload
  ${PROJECT_SOURCE_DIR}/dyncall
  ${PROJECT_SOURCE_DIR}/dyncallback
)

if (CMAKE_CXX_COMPILER_WORKS)
add_subdirectory(plain_c++)
add_subdirectory(suite2)
add_subdirectory(suite3)
add_subdirectory(suite_floats)
add_subdirectory(ellipsis)
endif ()

add_subdirectory(call_suite)
add_subdirectory(callback_suite)
add_subdirectory(plain)
add_subdirectory(suite)
add_subdirectory(callf)
add_subdirectory(nm)
add_subdirectory(dynload_plain)
add_subdirectory(resolve_self)
add_subdirectory(thunk)
add_subdirectory(malloc_wx)
add_subdirectory(callback_plain)
add_subdirectory(sharedlib)

if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86" AND CMAKE_CXX_COMPILER_WORKS)
add_subdirectory(suite_x86win32fast)
add_subdirectory(suite_x86win32std)
add_subdirectory(suite2_x86win32fast)
add_subdirectory(suite2_x86win32std)
endif()

if(${UNIX})
add_subdirectory(syscall)
endif()

