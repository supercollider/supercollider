# Package: dyncall
# File: dyncall/CMakeLists.txt
# Description: DynCall library cmake files
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

if(MSVC)
  if(CMAKE_SIZEOF_VOID_P MATCHES 4)
    set(ASM_SRC dyncall_call_x86_generic_masm.asm)	
  else()
    set(ASM_SRC dyncall_call_x64_generic_masm.asm)
  endif()
else()
  set(ASM_SRC dyncall_call.S)
  set_source_files_properties(${ASM_SRC} PROPERTIES LANGUAGE "C")
endif()

add_library(dyncall_s STATIC ${ASM_SRC} 
  dyncall_vector.c 
  dyncall_struct.c 
  dyncall_api.c 
  dyncall_callvm.c 
  dyncall_callvm_base.c
  dyncall_callf.c
)

install(TARGETS dyncall_s ARCHIVE DESTINATION lib )

install(FILES 
  dyncall_macros.h 
  dyncall_config.h 
  dyncall_types.h 
  dyncall.h
  dyncall_signature.h
  dyncall_value.h
  dyncall_callf.h
  DESTINATION include
)

