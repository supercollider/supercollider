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
SC_CLASS_LIBRARY = SCClassLibrary
install-data-local:
	@echo "*** installing class library ***" ; \
	cd "$(srcdir)" ; \
	find $(SC_CLASS_LIBRARY) -name CVS -prune -o -type d -print0 | \
		xargs -0 -i $(mkinstalldirs) $(SC_DATA_DIR)/{} ; \
	find $(SC_CLASS_LIBRARY) -name '*.sc' -print0 | \
		xargs -0 -i $(INSTALL_DATA) {} $(SC_DATA_DIR)/{} ;
uninstall-local:
	@find $(SC_CLASS_LIBRARY) -name '*.sc' -print0 | \
		xargs -0 -i rm -f $(SC_DATA_DIR)/{} ;
endif # WITH_LIBRARY

## EOF ================================================================
