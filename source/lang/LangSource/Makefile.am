## ====================================================================
## file:	Makefile.am
## contents:	automake template
## author:      stefan kersten <steve@k-hornz.de>
## CVS:         $Id$
## ====================================================================

CLEANFILES		= lang_tab.tmp.cpp
MAINTAINERCLEANFILES	= Makefile.in

INCLUDES		= -I$(top_srcdir)/headers/common \
			-I$(top_srcdir)/headers/plugin_interface \
			-I$(top_srcdir)/headers/lang -I$(top_srcdir)/headers/server \
			-I$(srcdir)/erase-compiler
AM_CPPFLAGS		= $(SC_CPPFLAGS) -DSC_LIBRARY_DIR=\"$(SC_LIBRARY_DIR)\" 
AM_CFLAGS		= $(SC_CFLAGS) $(SNDFILE_CFLAGS)
AM_CXXFLAGS		= $(AM_CFLAGS)

#lib_LTLIBRARIES		= libsclang.la
noinst_LTLIBRARIES	= libsclang.la
libsclang_la_LDFLAGS	= -version-info $(SC_LANG_SHARED_VERSION_INFO)
libsclang_la_LIBADD	= $(top_builddir)/source/common/libcommon.la \
			$(top_builddir)/source/lang/LangPrimSource/liblangprim.la \
			-lm -lpthread \
			$(SNDFILE_LIBS) $(DMALLOC_LIBS)

libsclang_la_SOURCES	=		\
	AdvancingAllocPool.cpp		\
	ByteCodeArray.cpp		\
	DumpParseNode.cpp		\
	GC.cpp				\
	InitAlloc.cpp			\
	PyrFileUtils.cpp		\
	PyrInterpreter3.cpp		\
	PyrLexer.cpp			\
	PyrMathOps.cpp			\
	PyrMathPrim.cpp			\
	PyrMathSupport.cpp		\
	PyrMessage.cpp			\
	PyrObject.cpp			\
	PyrParseNode.cpp		\
	PyrPrimitive.cpp		\
	PyrSched.cpp			\
	PyrSignal.cpp			\
	PyrSignalPrim.cpp		\
	PyrSymbolTable.cpp		\
	SC_ComPort.cpp			\
	SC_HID.cpp			\
	SC_CoreMIDI.cpp			\
	SC_TerminalClient.cpp		\
	Samp.cpp			\
	SimpleStack.cpp			\
	VMGlobals.cpp			\
	alloca.cpp			\
	dumpByteCodes.cpp		\
	lang_tab.tmp.cpp		\
	libraryConfig.cpp

bin_PROGRAMS		= sclang
sclang_LDADD		= libsclang.la $(top_srcdir)/source/server/libscsynth.la
sclang_SOURCES		= cmdLineFuncs.cpp

# copy parser source from erase-compiler
lang_tab.tmp.cpp: $(srcdir)/erase-compiler/lang11d_tab.cpp
	cp $< $@

## EOF ================================================================
