# Install script for directory: __SC_GITHUB_HOME__

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files/SuperCollider")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/SuperCollider" TYPE DIRECTORY FILES "__SC_GITHUB_HOME__/SCClassLibrary" REGEX "ignoreme" EXCLUDE REGEX "/[^/]*\~$" EXCLUDE REGEX "/[^/]*\#$" EXCLUDE)
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/SuperCollider" TYPE DIRECTORY FILES "__SC_GITHUB_HOME__/sounds")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/SuperCollider" TYPE DIRECTORY FILES "__SC_GITHUB_HOME__/examples")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/SuperCollider" TYPE FILE FILES
    "__SC_GITHUB_HOME__/README.md"
    "__SC_GITHUB_HOME__/README_WINDOWS.md"
    "__SC_GITHUB_HOME__/ChangeLog"
    "__SC_GITHUB_HOME__/COPYING"
    "__SC_GITHUB_HOME__/AUTHORS"
    )
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/SuperCollider" TYPE DIRECTORY FILES "__SC_GITHUB_HOME__/HelpSource" REGEX "ignoreme" EXCLUDE REGEX "/[^/]*\~$" EXCLUDE REGEX "/[^/]*\#$" EXCLUDE)
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("__SC_GITHUB_HOME__/vs2013/solution/external_libraries/cmake_install.cmake")
  include("__SC_GITHUB_HOME__/vs2013/solution/server/cmake_install.cmake")
  include("__SC_GITHUB_HOME__/vs2013/solution/lang/cmake_install.cmake")
  include("__SC_GITHUB_HOME__/vs2013/solution/platform/cmake_install.cmake")
  include("__SC_GITHUB_HOME__/vs2013/solution/editors/cmake_install.cmake")
  include("__SC_GITHUB_HOME__/vs2013/solution/testsuite/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "__SC_GITHUB_HOME__/vs2013/solution/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
