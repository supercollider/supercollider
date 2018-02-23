#!/bin/sh

cd `dirname $0`/../../

# Build libraries.

make -f Makefile.generic clean all

# Build tests: dynload tests need '-ldl'.

( cd test ; make -f Makefile.generic clean )
( cd test ; LDLIBS=-ldl make -f Makefile.generic all-dynload )
( cd test ; make -f Makefile.generic all )


