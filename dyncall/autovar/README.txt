autovar -- a small macro auto-detection library


the following gives the list of variables available.
for each variable, a separate header file is used.

OS: Operating System
ARCH: Architecture
CC: C Compiler
ABI: Application Binary Interface
OSFAMILY: OS Roots


the following gives the tree of variables

OS: 
  Win32
  Win64
  Darwin
    IOS
    MacOSX
  Linux
  FreeBSD
  OpenBSD
  NetBSD
  DragonFlyBSD
  SunOS
  Cygwin
  MinGW
  NDS
  PSP
  BeOS
  Plan9
  VMS
  Minix
  Unknown

ARCH:
  X86
  X64
  IA64
  PPC
  PPC64
  MIPS64
  MIPS
  ARM
    THUMB
  SH
  SPARC64
  SPARC

ABI:
  PE
  Mach
  ELF
    ELF32
    ELF64

OSFAMILY: 
  Windows
  Unix
  GameConsole

CC:
  INTEL
  MSVC
  GNU
  WATCOM
  PCC
  SUN
