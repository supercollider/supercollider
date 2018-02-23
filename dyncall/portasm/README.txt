portasm - a toolkit for writing portable generic assembler sources 
------------------------------------------------------------------

Copyright (C) 2011 Daniel Adler <dadler@uni-goettingen.de>.
Licensed under BSD two-clause license.


Requirements
------------
- C Preprocessor


Supported Architectures and Tool-chains:
----------------------------------------

- x86: gas, apple as, masm
- x64: gas, apple as, masm
- ppc: gas, apple as
- arm: gas, apple as


Usage:
------

Implement assembler sources in *.S files which use C preprocessor.
#include portasm-<ARCH>.S at front

In order to generate MASM files for X86 and X64, run 
  'gen-masm.sh <name>' script which reads <name>.S and outputs <name>.masm file.


Common Macros:
--------------

BEGIN_ASM
END_ASM
BEGIN_PROC(name)
END_PROC(name)
GLOBAL(name)
HEX(value)


