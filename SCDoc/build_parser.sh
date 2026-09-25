#!/bin/sh

# macOS has its own, outdated, version of bison
# which will generate problems.
# You can use a more up-to-date version via brew
# and use it via $(brew --prefix bison)/bin/bison

flex -P scdoc -o lex.scdoc.cpp SCDoc.l &&
bison -p scdoc --defines -o SCDoc.tab.cpp SCDoc.y
#g++ -g -O3 -o scdoc_parse_dump lex.scdoc.cpp SCDoc.tab.cpp SCDoc.cpp main.cpp -ll

