## ====================================================================
## file:	Makefile.am
## contents:	automake template
## author:      stefan kersten <steve@k-hornz.de>
## CVS:         $Id$
## ====================================================================

MAINTAINERCLEANFILES	= Makefile.in

INCLUDES		= $(SC_LANG_INCLUDES)
AM_CPPFLAGS		= $(SC_CPPFLAGS)
AM_CFLAGS		= $(SC_CFLAGS) $(SC_SNDFILE_CFLAGS)
AM_CXXFLAGS		= $(AM_CFLAGS)

noinst_LTLIBRARIES	= liblangprim.la

liblangprim_la_SOURCES	=		\
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
