## ====================================================================
## file:	Makefile.am
## contents:	automake template
## author:      stefan kersten <steve@k-hornz.de>
## CVS:         $Id$
## ====================================================================

MAINTAINERCLEANFILES	= Makefile.in

AM_CXXFLAGS		= -I$(top_srcdir)/headers/common \
			-I$(top_srcdir)/headers/plugin_interface \
			-I$(top_srcdir)/headers/server
noinst_LTLIBRARIES	= libcommon.la
libcommon_la_SOURCES = 	\
SC_AllocPool.cpp	\
SC_Sem.cpp		\
SC_StringParser.cpp	\
scsynthsend.cpp

## EOF ================================================================
