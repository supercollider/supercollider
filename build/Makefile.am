## ====================================================================
## file:	Makefile.am
## contents:	automake template
## author:      stefan kersten <steve@k-hornz.de>
## CVS:         $Id$
## ====================================================================

MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST = Help SCClassLibrary TestingAndToDo examples

# install class library
install-data-local:
	@for f in `find SCClassLibrary -name '*.sc'`; do \
		d=`echo $$f | sed -e 's,/[^/]*$$,,'`; \
		$(mkinstalldirs) $(pkgdatadir)/$$d; \
		echo "installing $$f"; \
		$(INSTALL_DATA) $$f $(pkgdatadir)/$$f; \
	done

uninstall-local:
	@rm -rf $(pkgdatadir)/SCClassLibrary

## EOF ================================================================
