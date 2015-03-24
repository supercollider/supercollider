# Install script for directory: C:/Users/lucas/Documents/GitHub/supercollider

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/SuperCollider" TYPE DIRECTORY FILES "C:/Users/lucas/Documents/GitHub/supercollider/SCClassLibrary" REGEX "ignoreme" EXCLUDE REGEX "/[^/]*\\~$" EXCLUDE REGEX "/[^/]*\\#$" EXCLUDE)
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/SuperCollider" TYPE DIRECTORY FILES "C:/Users/lucas/Documents/GitHub/supercollider/sounds")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/SuperCollider" TYPE DIRECTORY FILES "C:/Users/lucas/Documents/GitHub/supercollider/examples")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/SuperCollider" TYPE FILE FILES
    "C:/Users/lucas/Documents/GitHub/supercollider/README.md"
    "C:/Users/lucas/Documents/GitHub/supercollider/README_WINDOWS.md"
    "C:/Users/lucas/Documents/GitHub/supercollider/ChangeLog"
    "C:/Users/lucas/Documents/GitHub/supercollider/COPYING"
    "C:/Users/lucas/Documents/GitHub/supercollider/AUTHORS"
    )
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/SuperCollider" TYPE DIRECTORY FILES "C:/Users/lucas/Documents/GitHub/supercollider/HelpSource" REGEX "ignoreme" EXCLUDE REGEX "/[^/]*\\~$" EXCLUDE REGEX "/[^/]*\\#$" EXCLUDE)
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("C:/Users/lucas/Documents/GitHub/supercollider/vs/build/external_libraries/cmake_install.cmake")
  include("C:/Users/lucas/Documents/GitHub/supercollider/vs/build/server/cmake_install.cmake")
  include("C:/Users/lucas/Documents/GitHub/supercollider/vs/build/lang/cmake_install.cmake")
  include("C:/Users/lucas/Documents/GitHub/supercollider/vs/build/platform/cmake_install.cmake")
  include("C:/Users/lucas/Documents/GitHub/supercollider/vs/build/editors/cmake_install.cmake")
  include("C:/Users/lucas/Documents/GitHub/supercollider/vs/build/testsuite/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "C:/Users/lucas/Documents/GitHub/supercollider/vs/build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
