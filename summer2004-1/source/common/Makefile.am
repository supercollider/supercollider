## ====================================================================
## file:	Makefile.am
## contents:	automake template
## author:      stefan kersten <steve@k-hornz.de>
## CVS:         $Id$
## ====================================================================

MAINTAINERCLEANFILES	= Makefile.in

INCLUDES		= $(SC_SYNTH_INCLUDES)
AM_CPPFLAGS		= $(SC_CPPFLAGS)
AM_CFLAGS		= $(SC_CFLAGS)
AM_CXXFLAGS		= $(AM_CFLAGS)

noinst_LTLIBRARIES	= libcommon.la
libcommon_la_SOURCES	= \
	SC_AllocPool.cpp \
	SC_Sem.cpp \
	SC_StringBuffer.cpp \
	SC_StringParser.cpp \
	scsynthsend.cpp

## EOF ================================================================
