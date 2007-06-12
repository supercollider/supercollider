## ====================================================================
## file:	Makefile.am
## contents:	automake template
## author:      stefan kersten <steve@k-hornz.de>
## CVS:         $Id$
## ====================================================================

MAINTAINERCLEANFILES	= Makefile.in

INCLUDES		= $(SC_SYNTH_INCLUDES)
AM_CPPFLAGS		= $(SC_CPPFLAGS) -DSC_AUDIO_API=$(SC_AUDIO_API) \
			-DSC_PLUGIN_DIR=\"$(SC_PLUGIN_DIR)\" \
			-DSC_PLUGIN_EXT=\"$(SC_PLUGIN_EXT)\" \
			-DSC_PLUGIN_LOAD_SYM=\"$(SC_PLUGIN_LOAD_SYM)\"
AM_CFLAGS		= $(SC_CFLAGS) $(SC_AUDIO_API_CFLAGS) $(SC_SNDFILE_CFLAGS)
AM_CXXFLAGS		= $(AM_CFLAGS)

lib_LTLIBRARIES		= libscsynth.la
libscsynth_la_LDFLAGS	= -version-info $(SC_SYNTH_SHARED_VERSION_INFO)
libscsynth_la_LIBADD	= $(top_builddir)/source/common/libcommon.la \
			$(SC_LIBS) $(SC_AUDIO_API_LIBS) $(SC_SNDFILE_LIBS)
libscsynth_la_SOURCES	= \
	Samp.cpp \
	SC_BufGen.cpp \
	SC_Carbon.cpp \
	SC_Complex.cpp \
	SC_ComPort.cpp \
	SC_CoreAudio.cpp \
	SC_Dimension.cpp \
	SC_Errors.cpp \
	SC_Graph.cpp \
	SC_GraphDef.cpp \
	SC_Group.cpp \
	SC_Lib_Cintf.cpp \
	SC_Lib.cpp \
	SC_MiscCmds.cpp \
	SC_Node.cpp \
	SC_Rate.cpp \
	SC_SequencedCommand.cpp \
	SC_Str4.cpp \
	SC_SyncCondition.cpp \
	SC_Unit.cpp \
	SC_UnitDef.cpp \
	SC_World.cpp

bin_PROGRAMS		= scsynth
scsynth_LDADD		= libscsynth.la
scsynth_SOURCES		= scsynth_main.cpp

## EOF ================================================================
