# Package: dyncall
# File: CMakeLists.txt
# Description: DynCall Project cmake files
# License:
#
# Copyright (c) 2010 Daniel Adler <dadler@uni-goettingen.de>
#
# Permission to use, copy, modify, and distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#

cmake_minimum_required (VERSION 2.6)
project(DynCall C)

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "dyncall enables dynamic invocation of machine-level function calls")
set(CPACK_PACKAGE_VENDOR "dyncall project")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/doc/Description.txt")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/doc/License.txt")
if(APPLE)
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/doc/Description.txt")
endif(APPLE)
set(CPACK_PACKAGE_NAME "dyncall")
include(CPack)

# add cmake modules shipped with the package to cmake's module path.
# currently we have no use for this, but maybe later..
#set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/buildsys/cmake/Modules" "${CMAKE_MODULE_PATH}")

option(LANG_CXX "Enable and build C++ tests" ON)

if(LANG_CXX)
  enable_language(CXX)
endif()

include(CheckCCompilerFlag)
check_c_compiler_flag("-fPIC -Werror" COMPILER_HAS_FPIC)

if(MSVC)
  enable_language(ASM_MASM)
  #if(CMAKE_SIZEOF_VOID_P MATCHES 8)
  #  set(CMAKE_ASM_COMPILER "ml64")
  #else()
  #  set(CMAKE_ASM_COMPILER "ml")
  #endif()
  #set(CMAKE_ASM_COMPILER_ARG1 "/c")
  #set(CMAKE_ASM_MASM_SOURCE_FILE_EXTENSIONS asm)
  #set(CMAKE_ASM_MASM_COMPILE_OBJECT "<CMAKE_ASM_MASM_COMPILER> <FLAGS> /c /Fo <OBJECT> <SOURCE>")
elseif(CMAKE_COMPILER_IS_GNUCC OR "${CMAKE_C_COMPILER_ID}" STREQUAL "Intel")
  if(COMPILER_HAS_FPIC)
    # when used in shared libraries, -fPIC is required by several architectures
    # and platforms (observed on AMD64, Solaris/Sparc).
    # we enable it per default here.
    add_definitions("-fPIC")
  endif()
  # enable gcc as front-end to assembler for .S files
  set(CMAKE_ASM_COMPILER "${CMAKE_C_COMPILER}")
  set(CMAKE_ASM_COMPILER_ARG1 "${CPPFLAGS} -c")
  enable_language(ASM)
elseif(CMAKE_COMPILER_IS_CLANG)
elseif("${CMAKE_C_COMPILER_ID}" STREQUAL "SunPro")
  set(CMAKE_ASM_COMPILER "${CMAKE_C_COMPILER}")
  set(CMAKE_ASM_COMPILER_ARG1 "${CPPFLAGS} -c")
  enable_language(ASM)
else()
  if (CMAKE_C_COMPILER_ID STREQUAL "Clang")
    if(COMPILER_HAS_FPIC)
        add_definitions("-fPIC")
    endif()
    # enable gcc as front-end to assembler for .S files
    set(CMAKE_ASM_COMPILER "${CMAKE_C_COMPILER}")
    set(CMAKE_ASM_COMPILER_ARG1 "${CPPFLAGS} -c")
    enable_language(ASM)
  endif()
endif()

add_subdirectory(dynload)
add_subdirectory(dyncall)
add_subdirectory(dyncallback)
add_subdirectory(doc/manual EXCLUDE_FROM_ALL)
add_subdirectory(test EXCLUDE_FROM_ALL)

# install cmake Find scripts (disabled per default)

option(INSTALL_CMAKE_MODULES "install cmake modules to locate dyncall" "NO")

if(INSTALL_CMAKE_MODULES)
file(GLOB INSTALL_CMAKE_MODULES_FILES buildsys/cmake/Modules/Find*.cmake)
install(FILES ${INSTALL_CMAKE_MODULES_FILES} DESTINATION ${CMAKE_ROOT}/Modules)
endif(INSTALL_CMAKE_MODULES)
