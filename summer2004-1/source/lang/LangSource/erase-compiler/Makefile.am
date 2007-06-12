## ====================================================================
## file:	Makefile.am
## contents:	automake template
## author:      stefan kersten <steve@k-hornz.de>
## CVS:         $Id$
## ====================================================================

MAINTAINERCLEANFILES	= Makefile.in

INCLUDES		= -I$(top_srcdir)/headers/common \
			-I$(top_srcdir)/headers/plugin_interface \
			-I$(top_srcdir)/headers/lang \
			-I$(top_srcdir)/headers/server
AM_CPPFLAGS		= $(SC_CPPFLAGS)
AM_CFLAGS		= $(SC_CFLAGS)
AM_CXXFLAGS		= $(AM_CFLAGS)

noinst_LTLIBRARIES	= libparser.la
libparser_la_SOURCES	= lang11d_tab.cpp lang11d_tab.h

## EOF ================================================================
