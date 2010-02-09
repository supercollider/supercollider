ifeq ($(PROCESSOR),PPU)
	CC     :=  gcc
	AR     :=  ar -r
	CFLAGS :=  -D__PPU__ -mabi=altivec -maltivec 
	CFLAGS +=  --param max-inline-insns-single=1200 -Winline
else
# PROCESSOR must be one of SSE, PPU, or SPU

ifeq ($(PROCESSOR),SPU)
	CC     :=  spu-gcc
	AR     :=  spu-ar -r
	CFLAGS :=  --param max-inline-insns-single=1200 -Winline
else
ifeq ($(PROCESSOR),SSE)
	CC     :=  gcc
	AR     :=  ar -r
	CFLAGS :=  -msse3
endif
endif
endif

Header := asinf4.h atanf4.h expf4.h fabsf4.h floorf4.h  logf4.h \
	  powf4.h simdconst.h simdmath.h sincosf4.h sqrtf4.h


lib:	Makefile math.c   $(Header)
	$(CC) $(CFLAGS) -O3 -c math.c
	$(AR) libsimdmath.a math.o


clean:	
	rm -rf *.o libsimdmath.a
