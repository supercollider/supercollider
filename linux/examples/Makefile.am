## ====================================================================
## file:	Makefile.am
## contents:	automake template
## author:      stefan kersten <steve@k-hornz.de>
## CVS:         $Id$
## ====================================================================

DISTCLEANFILES		= sclang.cfg sclang.cfg.tmp
MAINTAINERCLEANFILES	= Makefile.in

.PHONY: sclang.cfg

sclang.cfg:
	rm -f sclang.cfg sclang.cfg.tmp
	sed -e 's,@SC_LIB_DIR\@,$(SC_LIB_DIR),g' $(srcdir)/sclang.cfg.in > sclang.cfg.tmp
	mv sclang.cfg.tmp sclang.cfg

install-data-local: sclang.cfg
	$(mkinstalldirs) $(SC_LIB_DIR)/Linux
	$(INSTALL_DATA) $(srcdir)/extMain.sc $(SC_LIB_DIR)/Linux/extMain.sc

uninstall-local:
	rm -f $(libdir)/Linux/extMain.sc

## EOF ================================================================
