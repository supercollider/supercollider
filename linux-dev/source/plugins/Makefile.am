## ====================================================================
## file:	Makefile.am
## contents:	automake template
## author:      stefan kersten <steve@k-hornz.de>
## CVS:         $Id$
## ====================================================================

MAINTAINERCLEANFILES = Makefile.in

AM_CXXFLAGS		= -I$(top_srcdir)/headers/common \
			-I$(top_srcdir)/headers/plugin_interface \
			-I$(top_srcdir)/headers/server
plugindir		= $(SC_PLUGIN_DIR)
plugin_LTLIBRARIES = 		\
	BinaryOpUGens.la	\
	DelayUGens.la		\
	DiskIO_UGens.la		\
	FFT_UGens.la		\
	FilterUGens.la		\
	IOUGens.la		\
	LFUGens.la		\
	MulAddUGens.la		\
	NoiseUGens.la		\
	OscUGens.la		\
	PanUGens.la		\
	TriggerUGens.la		\
	UnaryOpUGens.la

# BinaryOpUGens
BinaryOpUGens_la_SOURCES	= BinaryOpUGens.cpp
BinaryOpUGens_la_LDFLAGS	= -module -no-undefined -avoid-version

# DelayUGens
DelayUGens_la_SOURCES		= DelayUGens.cpp
DelayUGens_la_LDFLAGS		= -module -no-undefined -avoid-version

# DiskIO_UGens
DiskIO_UGens_la_SOURCES		= DiskIO_UGens.cpp
DiskIO_UGens_la_LDFLAGS		= -module -no-undefined -avoid-version

# FFT_UGens
FFT_UGens_la_SOURCES		= FFT_UGens.cpp				\
				fftlib.c fftlib.h			\
				SCComplex.cpp SCComplex.h
FFT_UGens_la_LDFLAGS		= -module -no-undefined -avoid-version

# FilterUGens
FilterUGens_la_SOURCES		= FilterUGens.cpp
FilterUGens_la_LDFLAGS		= -module -no-undefined -avoid-version

# IOUGens
IOUGens_la_SOURCES		= IOUGens.cpp
IOUGens_la_LDFLAGS		= -module -no-undefined -avoid-version

# LFUGens
LFUGens_la_SOURCES		= LFUGens.cpp
LFUGens_la_LDFLAGS		= -module -no-undefined -avoid-version

# MulAddUGens
MulAddUGens_la_SOURCES		= MulAddUGens.cpp
MulAddUGens_la_LDFLAGS		= -module -no-undefined -avoid-version

# NoiseUGens
NoiseUGens_la_SOURCES		= NoiseUGens.cpp
NoiseUGens_la_LDFLAGS		= -module -no-undefined -avoid-version

# OscUGens
OscUGens_la_SOURCES		= OscUGens.cpp
OscUGens_la_LDFLAGS		= -module -no-undefined -avoid-version

# PanUGens
PanUGens_la_SOURCES		= PanUGens.cpp
PanUGens_la_LDFLAGS		= -module -no-undefined -avoid-version

# TriggerUGens
TriggerUGens_la_SOURCES		= TriggerUGens.cpp
TriggerUGens_la_LDFLAGS		= -module -no-undefined -avoid-version

# UnaryOpUGens
UnaryOpUGens_la_SOURCES		= UnaryOpUGens.cpp
UnaryOpUGens_la_LDFLAGS		= -module -no-undefined -avoid-version

## EOF ================================================================
