## ====================================================================
## file:	Makefile.am
## contents:	automake template
## author:      stefan kersten <steve@k-hornz.de>
## CVS:         $Id$
## ====================================================================

DISTCLEANFILES		= libscsynth.pc libsclang.pc
MAINTAINERCLEANFILES	= Makefile.in

if WITH_SCEL
sceldir			= scel
else
sceldir			=
endif

SUBDIRS			= examples $(sceldir)
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
