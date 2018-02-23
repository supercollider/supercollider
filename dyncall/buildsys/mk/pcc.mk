#//////////////////////////////////////////////////////////////////////////////
#
# Copyright (c) 2010 Daniel Adler <dadler@uni-goettingen.de>, 
#                    Tassilo Philipp <tphilipp@potion-studios.com>
#
# Permission to use, copy, modify, and distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#
#//////////////////////////////////////////////////////////////////////////////


#@@@ add stuff for crosscompiling here.

CC = pcc
LD = pcc

#@@@.if $(BUILD_CONFIG) == "debug"
#@@@ add -g option or similar
#@@@.endif
CPPFLAGS = -D__Plan9__ -D__${objtype}__
CFLAGS   = -D__Plan9__ -D__${objtype}__ -I$TOP/dyncall -I$TOP/dyncallback -c
#CXXFLAGS = $CXXFLAGS -D__Plan9__
#ASFLAGS  = -D__Plan9__

# JUST TEMPORARY AS LONG AS I'M WORKING ON IT - Plan9's sed doesn't support sed comments
#        /^$/d                                                               # Remove empty lines.
#        /^#.*/d                                                             # Remove comments.
#        s/^\.(globl|intel_syntax|file|section).*//                          # Remove some GAS directives.
#        s/(.*):/TEXT \1(SB), $0/g                                           # Reformat function names.
#        s/%//g                                                              # Remove % register prefixes.
#        /^[  ]+/y/abcdefghijklmnopqrstuvwxyz/ABCDEFGHIJKLMNOPQRSTUVWXYZ/    # Convert everything to uppercase.
#        s/([A-Z]+)[  ]*E(..)[,   ]*E(..)/\1L \3, \2/                        # Convert 32bit instructions with 2 register operands.
#        s/([A-Z]+)[  ]*E(..)[,   ]*([0-9]+)/\1L $\3, \2/                    # Convert 32bit instructions with register and constant as
#        s/([A-Z]+)[  ]*E(..)[,   ]*\[E(..)\+([0-9]+)\]/\1L \4(\3), \2/      # Convert 32bit instructions with register and address as 
#        s/([A-Z]+)[  ]*E(..)[,   ]*\[E(..)\]/\1L 0(\3), \2/                 # Convert 32bit instructions with register and address as 
#        s/(CALL)[    ]*E(..)[    ]*$/\1 \2/                                 # Convert CALL instructions.
#        s/(CALL)[    ]*\[E(..)\+([0-9]+)\]/\1 \3(\2)/                       # Convert CALL instructions with addressing mode.
#        s/([A-Z]+)[  ]*E(..)[    ]*$/\1L \2/                                # Convert 32bit instructions with register operand.
#        s/([A-Z]+)[  ]*\[E(..)\+([0-9]+)\]/\1L \3(\2)/                      # Convert 32bit instructions with address operand.
#        s/([A-Z]+)[  ]*\[E(..)\]/\1L 0(\2)/                                 # Convert 32bit instructions with address operand.
#        s/(REP)[     ]*(MOV)SB[  ]*$/\1; \2B SI, DI/                        # Convert repeater prefix stuff.
#        s/(REP)[     ]*(MOV)SD[  ]*$/\1; \2L SI, DI/                        # Convert repeater prefix stuff.

# Step to transform .S into object files.
%.$O: %.S
	cp $prereq $prereq.c
	pcc -E $CPPFLAGS $prereq.c > $stem.s # replace with cpp? pcc requires .c suffix
	rm $prereq.c
	$AS $AFLAGS $stem.s
	rm $stem.s

