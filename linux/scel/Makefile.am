## ====================================================================
## file:	Makefile.am
## contents:	automake template
## author:      stefan kersten <steve@k-hornz.de>
## CVS:         $Id$
## ====================================================================

# DISTCLEANFILES = sclang.cfg sclang.cfg.tmp
MAINTAINERCLEANFILES = Makefile.in

SUBDIRS = el sc
EXTRA_DIST = README

# install-data-local: sclang.cfg.in
# 	rm -f sclang.cfg sclang.cfg.tmp
# if WITH_SCEL
# 	sed -e 's,@datadir\@,$(pkgdatadir),g' $(srcdir)/sclang.cfg.in > sclang.cfg.tmp
# else
# 	sed -e 's,\(.*@datadir\@.*\),##\1,' $(srcdir)/sclang.cfg.in > sclang.cfg.tmp
# endif
# 	mv sclang.cfg.tmp sclang.cfg
# 	$(mkinstalldirs) /etc/SuperCollider
# 	$(INSTALL_DATA) sclang.cfg /etc/SuperCollider/sclang.scel.cfg

## EOF ================================================================
