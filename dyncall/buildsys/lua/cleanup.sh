#!/bin/sh

if [ -d lua-5.1.4 ]; then
  (cd lua-5.1.4;make clean)
fi

