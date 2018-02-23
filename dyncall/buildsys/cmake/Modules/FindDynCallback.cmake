# - Try to find DynLoad library
#
#   The following variables will be defined:
# 
#     DYNCALLBACK_FOUND
#     DYNCALLBACK_INCLUDE_DIRS
#     DYNCALLBACK_LIBRARIES
#
# ----------------------------------------------------------------------------
#
# Package: dyncall
# File: buildsys/cmake/Modules/FindDynLoad.cmake
# Description: CMake Module to find dynload library
# License:
#
# Copyright (c) 2010,2011 Daniel Adler <dadler@uni-goettingen.de>
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

find_path(DYNCALLBACK_INCLUDE_DIR NAMES dyncall_callback.h)
find_library(DYNCALLBACK_LIBRARY dyncallback_s)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(DynCallback DEFAULT_MSG DYNCALLBACK_LIBRARY DYNCALLBACK_INCLUDE_DIR)

if(DYNCALLBACK_FOUND)
  set(DYNCALLBACK_INCLUDE_DIRS ${DYNCALLBACK_INCLUDE_DIR})
  set(DYNCALLBACK_LIBRARIES ${DYNCALLBACK_LIBRARY})
endif(DYNCALLBACK_FOUND)

mark_as_advanced(DYNCALLBACK_INCLUDE_DIR DYNCALLBACK_LIBRARY)

