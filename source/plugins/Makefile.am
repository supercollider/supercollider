## ====================================================================
## file:	Makefile.am
## contents:	automake template
## author:      stefan kersten <steve@k-hornz.de>
## CVS:         $Id$
## ====================================================================

MAINTAINERCLEANFILES =	\
	Makefile.in	\
	aclocal.m4	\
	config.cache	\
	config.guess	\
	config.h.in	\
	config.h.in~	\
	config.log      \
	config.sub	\
	configure	\
	depcomp         \
	install-sh	\
	ltmain.sh	\
	missing		\
	mkinstalldirs	\
	stamp-h.in	\
        stamp-h

SC_INCLUDE_DIR		= $(top_srcdir)/headers
INCLUDES		= -I$(SC_INCLUDE_DIR)/common \
			-I$(SC_INCLUDE_DIR)/plugin_interface \
			-I$(SC_INCLUDE_DIR)/server
AM_CPPFLAGS		= $(SC_CPPFLAGS)
AM_CFLAGS		= $(SC_CFLAGS)
AM_CXXFLAGS		= $(AM_CFLAGS)

if HAVE_X11
MACUGENS_LA		= MacUGens.la
else
MACUGENS_LA		=
endif

plugindir		= $(SC_PLUGIN_DIR)
plugin_LTLIBRARIES	= \
			BinaryOpUGens.la \
			DelayUGens.la \
			DemandUGens.la \
			DiskIO_UGens.la \
			FFT_UGens.la \
			FilterUGens.la \
			GendynUGens.la \
			IOUGens.la \
			LFUGens.la \
			$(MACUGENS_LA) \
			MulAddUGens.la \
			NoiseUGens.la \
			OscUGens.la \
			PV_ThirdParty.la \
			PanUGens.la \
			PhysicalModelingUGens.la \
			TriggerUGens.la \
			UnaryOpUGens.la

# BinaryOpUGens
BinaryOpUGens_la_SOURCES	= BinaryOpUGens.cpp
BinaryOpUGens_la_LDFLAGS	= -module -no-undefined -avoid-version

# DelayUGens
DelayUGens_la_SOURCES		= DelayUGens.cpp
DelayUGens_la_LDFLAGS		= -module -no-undefined -avoid-version

# DemandUGens
DemandUGens_la_SOURCES		= DemandUGens.cpp
DemandUGens_la_LDFLAGS		= -module -no-undefined -avoid-version

# DiskIO_UGens
DiskIO_UGens_la_SOURCES		= DiskIO_UGens.cpp
DiskIO_UGens_la_LDFLAGS		= -module -no-undefined -avoid-version

# FFT_UGens
FFT_UGens_la_SOURCES		= FFT_UGens.cpp \
				fftlib.c fftlib.h \
				SCComplex.cpp SCComplex.h
FFT_UGens_la_LDFLAGS		= -module -no-undefined -avoid-version

# FilterUGens
FilterUGens_la_SOURCES		= FilterUGens.cpp
FilterUGens_la_LDFLAGS		= -module -no-undefined -avoid-version

# GendynUGens
GendynUGens_la_SOURCES		= GendynUGens.cpp
GendynUGens_la_LDFLAGS		= -module -no-undefined -avoid-version

# IOUGens
IOUGens_la_SOURCES		= IOUGens.cpp
IOUGens_la_LDFLAGS		= -module -no-undefined -avoid-version

# LFUGens
LFUGens_la_SOURCES		= LFUGens.cpp
LFUGens_la_LDFLAGS		= -module -no-undefined -avoid-version

# MacUGens
MacUGens_la_SOURCES		= MacUGens.cpp
MacUGens_la_CXXFLAGS		= $(SC_X11_CFLAGS)
MacUGens_la_LDFLAGS		= -module -no-undefined -avoid-version
MacUGens_la_LIBADD		= $(SC_X11_LIBS)

# MulAddUGens
MulAddUGens_la_SOURCES		= MulAddUGens.cpp
MulAddUGens_la_LDFLAGS		= -module -no-undefined -avoid-version

# NoiseUGens
NoiseUGens_la_SOURCES		= NoiseUGens.cpp
NoiseUGens_la_LDFLAGS		= -module -no-undefined -avoid-version

# OscUGens
OscUGens_la_SOURCES		= OscUGens.cpp
OscUGens_la_LDFLAGS		= -module -no-undefined -avoid-version

# PV_ThirdParty
PV_ThirdParty_la_SOURCES	= Convolution.cpp \
				fftlib.c fftlib.h \
				SCComplex.cpp SCComplex.h \
				FFT2InterfaceTable.cpp \
				FeatureDetection.cpp \
				PV_ThirdParty.cpp
PV_ThirdParty_la_LDFLAGS	= -module -no-undefined -avoid-version

# PanUGens
PanUGens_la_SOURCES		= PanUGens.cpp
PanUGens_la_LDFLAGS		= -module -no-undefined -avoid-version

# PhysicalModelingUGens
PhysicalModelingUGens_la_SOURCES       	= PhysicalModelingUGens.cpp
PhysicalModelingUGens_la_LDFLAGS       	= -module -no-undefined -avoid-version

# TriggerUGens
TriggerUGens_la_SOURCES		= TriggerUGens.cpp
TriggerUGens_la_LDFLAGS		= -module -no-undefined -avoid-version

# UnaryOpUGens
UnaryOpUGens_la_SOURCES		= UnaryOpUGens.cpp
UnaryOpUGens_la_LDFLAGS		= -module -no-undefined -avoid-version

## EOF ================================================================
