## ====================================================================
## file:	Makefile.am
## contents:	automake template
## author:      stefan kersten <steve@k-hornz.de>
## CVS:         $Id$
## ====================================================================

MAINTAINERCLEANFILES	= Makefile.in

SUBDIRS			= erase-compiler

INCLUDES		= $(SC_LANG_INCLUDES) -I$(srcdir)/erase-compiler
AM_CPPFLAGS		= $(SC_CPPFLAGS)
AM_CFLAGS		= $(SC_CFLAGS) $(SC_SNDFILE_CFLAGS)
AM_CXXFLAGS		= $(AM_CFLAGS)

lib_LTLIBRARIES		= libsclang.la
libsclang_la_LDFLAGS	= -version-info $(SC_LANG_SHARED_VERSION_INFO)
libsclang_la_LIBADD	= $(top_builddir)/source/common/libcommon.la \
			$(top_builddir)/source/lang/LangPrimSource/liblangprim.la \
			$(top_builddir)/source/lang/LangSource/erase-compiler/libparser.la \
			$(SC_LIBS) $(SC_SNDFILE_LIBS) $(SC_ALSA_LIBS)

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
	SC_AlsaMIDI.cpp			\
	SC_ComPort.cpp			\
	SC_LID.cpp			\
	SC_LanguageClient.cpp		\
	SC_LibraryConfig.cpp		\
	SC_TerminalClient.cpp		\
	Samp.cpp			\
	SimpleStack.cpp			\
	VMGlobals.cpp			\
	alloca.cpp			\
	dumpByteCodes.cpp

bin_PROGRAMS		= sclang
sclang_LDADD		= libsclang.la $(top_builddir)/source/server/libscsynth.la
sclang_SOURCES		= cmdLineFuncs.cpp

## EOF ================================================================
