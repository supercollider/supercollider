# Install script for directory: C:/Users/lucas/Documents/GitHub/supercollider/editors/sc-ide

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
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/SuperCollider" TYPE EXECUTABLE PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES "C:/Users/lucas/Documents/GitHub/supercollider/vs2013/solution/editors/sc-ide/Debug/scide.exe")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/SuperCollider" TYPE EXECUTABLE PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES "C:/Users/lucas/Documents/GitHub/supercollider/vs2013/solution/editors/sc-ide/Release/scide.exe")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/SuperCollider" TYPE EXECUTABLE PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES "C:/Users/lucas/Documents/GitHub/supercollider/vs2013/solution/editors/sc-ide/MinSizeRel/scide.exe")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/SuperCollider" TYPE EXECUTABLE PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE FILES "C:/Users/lucas/Documents/GitHub/supercollider/vs2013/solution/editors/sc-ide/RelWithDebInfo/scide.exe")
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/SuperCollider/translations" TYPE FILE FILES
    "C:/Users/lucas/Documents/GitHub/supercollider/vs2013/solution/editors/sc-ide/scide_de.qm"
    "C:/Users/lucas/Documents/GitHub/supercollider/vs2013/solution/editors/sc-ide/scide_es.qm"
    "C:/Users/lucas/Documents/GitHub/supercollider/vs2013/solution/editors/sc-ide/scide_fr.qm"
    "C:/Users/lucas/Documents/GitHub/supercollider/vs2013/solution/editors/sc-ide/scide_ja.qm"
    "C:/Users/lucas/Documents/GitHub/supercollider/vs2013/solution/editors/sc-ide/scide_pt.qm"
    "C:/Users/lucas/Documents/GitHub/supercollider/vs2013/solution/editors/sc-ide/scide_ru.qm"
    "C:/Users/lucas/Documents/GitHub/supercollider/vs2013/solution/editors/sc-ide/scide_sl.qm"
    "C:/Users/lucas/Documents/GitHub/supercollider/vs2013/solution/editors/sc-ide/scide_sv.qm"
    "C:/Users/lucas/Documents/GitHub/supercollider/vs2013/solution/editors/sc-ide/scide_zh.qm"
    "C:/Users/lucas/Documents/GitHub/supercollider/vs2013/solution/editors/sc-ide/scide.qm"
    )
endif()

