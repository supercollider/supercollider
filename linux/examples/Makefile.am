## ====================================================================
## file:	Makefile.am
## contents:	automake template
## author:      stefan kersten <steve@k-hornz.de>
## CVS:         $Id$
## ====================================================================

MAINTAINERCLEANFILES	= Makefile.in

linuxlibdir		= $(pkgdatadir)/SCClassLibrary/Linux
linuxlib_DATA		= extMain.sc

EXTRA_DIST		= sclang.cfg

install-data-local:
	@if test -n "${HOME}" -a ! -e "${HOME}/.sclang.cfg"; then \
		$(INSTALL_DATA) sclang.cfg "${HOME}/.sclang.cfg"; \
	fi

## EOF ================================================================
