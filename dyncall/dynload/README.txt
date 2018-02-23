dynload / abstraction to run-time shared library services:

- loading/unloading into the current process
- symbol lookup
- enumerating symbol tables
- elf: support for DT_GNU_HASH


Todo:
- a.out format
- support for different kind of symbols
  (exports,imports,sections,constants...)

Notes on windows pe format:

File Extension: dll

Implementation:

1. Access to internals
   The handle returned by LoadLibrary() is a pointer to the PE header 
   (which is a DOS header).


Notes on elf:

File Extension: so
Variants: Two core classes are defined Elf 32- and 64-bit. 
Only one model is compiled in the library (the current run-time format).

Dynamic symbol table:

Symbol table layout:
1. Index 0 in any symbol table is used to represent undefined symbols.
As such, the first entry in a symbol table (index 0) is always completely
zeroed (type STT_NOTYPE), and is not used.

2. If the file contains any local symbols, the second entry (index 1)
the symbol table will be a STT_FILE symbol giving the name of the file.

3. Section symbols.

4. Register symbols.

5. Global symbols that have been reduced to local scope via a mapfile.

6. For each input file that supplied local symbols, a STT_FILE symbol
   giving the name of the input file is put in the symbol table, 
   followed by the symbols in question.

7. The global symbols immediately follow the local symbols in the
   symbol table. Local and global symbols are always kept separate
   in this manner, and cannot be mixed together.


Dynamic symbol table handling seem to be different among platforms.
Due to System V ABI, one get access to the dynamic symbol table through
the DT_HASH entry in "DYNAMIC" Program Header.

It does not work on x86 on a x86_64 linux 2.6 machine.

A closer look to the binaries in /usr/lib32 revealed, there are differences:

differences
 - elf32 has 21 sections
 - elf64 has (21 + 2) sections
     ".hash"
     ".eh_frame_hdr"
 -       elf64 has  ".rela.*" 
   while elf32 has  ".rel.*"
 
in common:
 - both have a ".gnu.hash" section


the ".gnu.hash" 


Idea: "GNU hash" method ([3])


Symbol Versioning:



OS supported prelinking:

linux has prelink
irix has quickstart
solaris has crle

sparc uses STT_REGISTER:
  STT_REGISTER is
    The Sparc architecture has a concept known as a "register symbol". These
    symbols are used to validate symbol/register usage, and can also be
    used to initialize global registers. Other architectures don't use these.







References:
[1] Levin, R. John: Linkers & Loader
[2] System V ABI
[3] The cost of elf symbol hashing: http://blogs.sun.com/ali/entry/the_cost_of_elf_symbol 
[4] GNU Hash ELF Section: http://blogs.sun.com/ali/entry/gnu_hash_elf_sections
[5] http://refspecs.freestandards.org/LSB_3.2.0/LSB-Core-generic/LSB-Core-generic/symversion.html
[6] elf: http://greek0.net/elf.html
[7] System V ABI Application Binary Interface - Draft 17 - December 2003 (SCO) : 
    http://sco.com/developers/gabi/latest/contents.html 

