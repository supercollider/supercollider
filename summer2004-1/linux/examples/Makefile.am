## ====================================================================
## file:	Makefile.am
## contents:	automake template
## author:      stefan kersten <steve@k-hornz.de>
## CVS:         $Id$
## ====================================================================

DISTCLEANFILES		= sclang.cfg sclang.cfg.tmp
MAINTAINERCLEANFILES	= Makefile.in

sclang.cfg: $(srcdir)/sclang.cfg.in
	rm -f sclang.cfg sclang.cfg.tmp
	sed -e 's,@SC_LIB_DIR\@,$(SC_LIB_DIR),g' $(srcdir)/sclang.cfg.in > sclang.cfg.tmp
	mv sclang.cfg.tmp sclang.cfg

install-data-local: sclang.cfg

## EOF ================================================================
