## ====================================================================
## file:	Makefile.am
## contents:	automake template
## author:      stefan kersten <steve@k-hornz.de>
## CVS:         $Id$
## ====================================================================

DISTCLEANFILES		= libscsynth.pc libsclang.pc
MAINTAINERCLEANFILES	= Makefile.in

if WITH_LIBRARY
linuxlibdir		= lib
else
linuxlibdir		=
endif # WITH_LIBRARY

if WITH_SCEL
sceldir			= scel
else
sceldir			=
endif # WITH_SCEL

SUBDIRS			= examples $(linuxlibdir) $(sceldir)
EXTRA_DIST		= \
	ChangeLog \
	INSTALL \
	NEWS \
	README \
	bootstrap \
	libscsynth.pc.in \
	libsclang.pc.in

pkgconfigdir		= $(libdir)/pkgconfig
pkgconfig_DATA		= libscsynth.pc libsclang.pc

## EOF ================================================================
