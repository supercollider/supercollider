# - Try to find dyncall library
#
#   The following variables will be defined:
#
#     DYNCALL_FOUND
#     DYNCALL_INCLUDE_DIRS
#     DYNCALL_LIBRARIES
#
# ----------------------------------------------------------------------------
#
# Package: dyncall
# File: buildsys/cmake/Modules/CMakeLists.txt
# Description: CMake Module to find DynCall library
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

find_path(DYNCALL_INCLUDE_DIR NAMES dyncall.h)
find_library(DYNCALL_LIBRARY dyncall_s)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(DynCall DEFAULT_MSG DYNCALL_LIBRARY DYNCALL_INCLUDE_DIR)

if(DYNCALL_FOUND)
  set(DYNCALL_INCLUDE_DIRS ${DYNCALL_INCLUDE_DIR})
  set(DYNCALL_LIBRARIES ${DYNCALL_LIBRARY})
endif(DYNCALL_FOUND)

mark_as_advanced(DYNCALL_INCLUDE_DIR DYNCALL_LIBRARY)

