Library Design

Overview:
1. Call Kernel
2. Utilities (memory and static sized vectors)
3. Call State Machine
4. Application Programming Interface
5. extension: high-level formatted C API (ellipsis style)


1. Call Kernel 

Assembly Implementation:

  gcc front-end: dyncall_call.S
    uses the C preprocessor
    will include the apropriate assembly source

  architecture/tool specific sources:
    dyncall_call_<ARCH>_<ASMTOOL>.[s|asm]


2. Utilities (memory and static sized vectors)

2.1 Memory Management

C Interface: dyncall_alloc.h


2.2 Static-sized Vector

C Interface: dyncall_vector.h

C Implementation: dyncall_vector.c


3. Call State Machine

C Implementation:
  Top-level: dynall_callvm.c
  Sub-levels: 
    dyncall_callvm_<ARCH>.c


4. Application Programming Interface

C Header:
  Top-level: dyncall.h
  Sub-level headers: 
    dyncall_macros.h 
    dyncall_config.h 
    dyncall_types.h 
C Implementation: dyncall_api.c


5. Extension: High-level C API (ellipsis style)

C Header:
  Top-level: dyncall_callf.h
  Sub-level headers:
    dyncall_value.h
    dyncall_signature.h
C Implementation: dyncall_callf.c


