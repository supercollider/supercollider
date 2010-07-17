# PROCESSOR must be one of SSE, PPU, or SPU

ifeq ($(PROCESSOR),PPU)
	CC     :=  gcc
	AR     :=  ar -r
	CFLAGS :=  -D__PPU__ -mabi=altivec -maltivec 
	CFLAGS +=  --param max-inline-insns-single=1200 -Winline
else
ifeq ($(PROCESSOR),SPU)
	CC     :=  spu-gcc
	AR     :=  spu-ar -r
	CFLAGS :=  --param max-inline-insns-single=1200 -Winline
else
ifeq ($(PROCESSOR),SSE)
	CC     :=  gcc
	AR     :=  ar -r
	CFLAGS :=  -msse3 -msse4.1
endif
endif
endif

CFLAGS += -I../lib

LIBSM  := ../lib/libsimdmath.a

Header := asinf4.h atanf4.h expf4.h fabsf4.h floorf4.h  logf4.h \
	  powf4.h ../lib/simdconst.h ../lib/simdmath.h sincosf4.h sqrtf4.h


test:	Makefile simdtest.c $(LIBSM)
	$(CC) $(CFLAGS) -DLIBSIMD  simdtest.c -lm $(LIBSM) -o test

tlibm:	Makefile simdtest.c stdmath.c
	$(CC) $(CFLAGS) -O3  -c stdmath.c
	$(CC) $(CFLAGS) -DLIBMATH  stdmath.o simdtest.c -lm -o tlibm

tps3m:	Makefile simdtest.c ps3math.c
	$(CC) $(CFLAGS) -I/usr/lib/gcc/ppu/4.1.1/include -O3  -c ps3math.c
	$(CC) $(CFLAGS)  -DCELL  ps3math.o simdtest.c -lm -o tps3m

tveclib:	Makefile simdtest.c	
	$(CC) $(CFLAGS)  -DMACVEC -framework accelerate simdtest.c -o tveclib


clean:
	rm -rf *.o test tlibm tps3m tveclib
