# Package: dyncall
# File: dyncallback/CMakeLists.txt
# Description: DynCallback library cmake files
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

include_directories(${PROJECT_SOURCE_DIR}/dyncall)

if(MSVC)
  if(CMAKE_SIZEOF_VOID_P MATCHES 4)
    set(ASM_SRC dyncall_callback_x86_masm.asm)	
  else()
    set(ASM_SRC dyncall_callback_x64_masm.asm)
  endif()
else()
  set(ASM_SRC dyncall_callback_arch.S)
  set_source_files_properties(${ASM_SRC} PROPERTIES LANGUAGE "C")
endif()

add_library(dyncallback_s STATIC ${ASM_SRC}
  dyncall_thunk.c
  dyncall_alloc_wx.c
  dyncall_args.c
  dyncall_callback.c
)

install(TARGETS dyncallback_s ARCHIVE DESTINATION lib )

install(FILES
  dyncall_thunk.h
  dyncall_thunk_x86.h
  dyncall_thunk_ppc32.h
  dyncall_thunk_x64.h
  dyncall_thunk_arm32.h
  dyncall_alloc_wx.h
  dyncall_args.h
  dyncall_callback.h
  DESTINATION include
)

