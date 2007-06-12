## ====================================================================
## file:	Makefile.am
## contents:	automake template
## author:      stefan kersten <steve@k-hornz.de>
## CVS:         $Id$
## ====================================================================

MAINTAINERCLEANFILES	= Makefile.in

AM_CXXFLAGS		= -I$(top_srcdir)/headers/common \
			-I$(top_srcdir)/headers/plugin_interface \
			-I$(top_srcdir)/headers/lang -I$(top_srcdir)/headers/server \
			@SNDFILE_CFLAGS@
noinst_LTLIBRARIES	= liblangprim.la
liblangprim_la_SOURCES =		\
	OSCData.cpp			\
	PyrArchiver.cpp			\
	PyrArrayPrimitives.cpp		\
	PyrBitPrim.cpp			\
	PyrCharPrim.cpp			\
	PyrFilePrim.cpp			\
	PyrListPrim.cpp			\
	PyrStringPrim.cpp		\
	PyrSymbolPrim.cpp		\
	PyrUnixPrim.cpp

## EOF ================================================================
