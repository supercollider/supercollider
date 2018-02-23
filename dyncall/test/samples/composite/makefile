TARGETS=args.s

#CC=gcc-mp-4.7 
#TARGET_ARCH=-march=corei7-avx 
#TARGET_ARCH=-march=corei7-avx -mavx -msse2avx
#-mno-vzeroupper
CFLAGS=-O3 
#-fomit-frame-pointer -fno-exceptions 
# -fno-rtti
all: ${TARGETS}
clean:
	rm -f ${TARGETS}
.c.s:
	${CC} -S $< -o $@ ${CFLAGS} ${TARGET_ARCH} 
