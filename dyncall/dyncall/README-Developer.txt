Build with GCC Tool-Chain:

  One assembly front-end *.S source file for all supported architectures:
  
  Usage:
    gcc -c dyncall_call.S -o dyncall_call.o

  Advantages:
  - works fine with universal binary builds (fat binaries), e.g.:
    gcc -arch i386 -arch ppc -arch x86_64 -c dyncall_call.S -o dyncall_call.o
    
  Details:
  Simplified assembly file compilation via using a gigantic C Preprocessor switch include.
  
  source file "dyncall_call.S" selects the appropriate GAS/Apple assembly file
  "dyncall_call_<arch>_<asmtool>.[sS]".
  
  archs so far:
  
  arm32_thumb		.s
  arm32_arm		.s
  mips		.s
  ppc32		.s
  x64			.s
  x86			.S  [ uses C macros ]
  
  asmtools:
  
  gas	  - standard GNU assembler
  apple - apple's assembler (based on GNU but is significantly different in syntax)
  masm  - Microsoft assembler x86 and x64
  nasm  - Netwide assembler for x86 and x64
  
  
  NOTE: .S is used for preprocessing assembly files using gcc
        .s is used directly with as
      
advantages:
  - one way to build the kernel: 
  
     gcc -c dyncall_call.S -o dyncall_call.o
     
  - we can build now universal binaries


