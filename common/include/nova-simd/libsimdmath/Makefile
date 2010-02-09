# PROCESSOR must be one of SSE, PPU, or SPU
PROCESSOR := SSE


export PROCESSOR

all:
	cd lib   && $(MAKE)
	cd tests && $(MAKE) test
	cd tests && $(MAKE) tlibm
#	cd tests && $(MAKE) tps3m
#	cd tests && $(MAKE) tveclib

clean:	
	cd lib   && $(MAKE) clean
	cd tests && $(MAKE) clean
