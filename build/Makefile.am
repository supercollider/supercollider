## ====================================================================
## file:	Makefile.am
## contents:	automake template
## author:      stefan kersten <steve@k-hornz.de>
## CVS:         $Id$
## ====================================================================

MAINTAINERCLEANFILES	= Makefile.in
EXTRA_DIST		= \
	Help \
	SCClassLibrary \
	TestingAndToDo \
	examples \
	plugins \
	server \
	sounds \
	synthdefs

# install class library
if WITH_LIBRARY
install-data-local:
	@echo "*** installing class library ***"; \
	cd "$(srcdir)"; \
	for f in `find SCClassLibrary -name '*.sc'`; do \
		d=`echo $$f | sed -e 's,/[^/]*$$,,'`; \
		echo "==> $$f"; \
		$(mkinstalldirs) $(pkgdatadir)/$$d >/dev/null; \
		$(INSTALL_DATA) $$f $(pkgdatadir)/$$f >/dev/null; \
	done;

uninstall-local:
	@for f in `find $(SC_LIB_DIR) -name '*.sc'`; do \
		rm -f $$f; \
	done;
endif # WITH_LIBRARY

## EOF ================================================================
