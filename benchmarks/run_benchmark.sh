#!/bin/sh

./$1 > ./$1.dat
cat ./$1.dat| nl | sort -n -k 2