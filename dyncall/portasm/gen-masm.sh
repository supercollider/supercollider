#!/bin/sh
gcc -E -P -DGEN_MASM $1.S >$1.asm

