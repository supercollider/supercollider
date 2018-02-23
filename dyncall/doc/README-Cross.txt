Cross-compilation of dyncall with gcc
=====================================

Make sure cross-compile tools are installed and in your path, and set the
CC, AR and LD envvars, explicitly (where $PREFIX is arm-eabi, etc., depending
on the toolchain to be used):

  $ export CC=$PREFIX-gcc
  $ export AR=$PREFIX-ar
  $ export LD=$PREFIX-ld

Also set CXX if you want to build the tests:

  $ export CXX=$PREFIX-g++


Then, build dyncall - one way would be to simply use Makefile.embedded:

  $ make -f Makefile.embedded sun
  $ (cd test; make -f Makefile.embedded)

