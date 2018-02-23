# - Try to find dynload library
#
#   The following variables will be defined:
#
#     DYNLOAD_FOUND
#     DYNLOAD_INCLUDE_DIRS
#     DYNLOAD_LIBRARIES
#
# ----------------------------------------------------------------------------
#
# Package: dyncall
# File: buildsys/cmake/Modules/FindDynLoad.cmake
# Description: CMake Module to find dynload library
# License:
#
# Copyright (c) 2010-2011 Daniel Adler <dadler@uni-goettingen.de>
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

find_path(DYNLOAD_INCLUDE_DIR NAMES dynload.h)
find_library(DYNLOAD_LIBRARY dynload_s)

mark_as_advanced(DYNLOAD_INCLUDE_DIR DYNLOAD_LIBRARY)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(DynLoad DEFAULT_MSG DYNLOAD_LIBRARY DYNLOAD_INCLUDE_DIR)

if(DYNLOAD_FOUND)
  set(DYNLOAD_INCLUDE_DIRS ${DYNLOAD_INCLUDE_DIR})
  set(DYNLOAD_LIBRARIES ${DYNLOAD_LIBRARY})
endif(DYNLOAD_FOUND)

mark_as_advanced(DYNLOAD_INCLUDE_DIR DYNLOAD_LIBRARY)

