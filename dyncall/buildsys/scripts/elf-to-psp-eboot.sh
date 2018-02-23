#!/bin/sh

if ! [ -x "$1" ]; then
	echo Usage: $0 \<elf\>
	exit 1
fi

TITLE=`basename $1`
SFO=/tmp/${TITLE}.sfo
FIXED_ELF=/tmp/${TITLE}.fixed
STRIPPED_ELF=/tmp/${TITLE}.stripped

mksfo             dyncall_${TITLE} $SFO
psp-fixup-imports -o $FIXED_ELF $1
psp-strip         $FIXED_ELF -o $STRIPPED_ELF
pack-pbp          EBOOT.PBP $SFO NULL NULL NULL NULL NULL $STRIPPED_ELF NULL
rm $SFO
rm $FIXED_ELF
rm $STRIPPED_ELF
