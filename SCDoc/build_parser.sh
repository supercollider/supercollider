#!/bin/sh
flex -P scdoc -o lex.scdoc.cpp SCDoc.l &&
bison -p scdoc --defines -o SCDoc.tab.cpp SCDoc.y
#g++ -g -O3 -o scdoc_parse_dump lex.scdoc.cpp SCDoc.tab.cpp SCDoc.cpp main.cpp -ll

