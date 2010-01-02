# setup.py for use with py2exe to create the python module PySCLang
# Created in March 2008 for SC 3.2 by John Glover (glover.john@gmail.com)

# Usage: edit lines 13 and 16 below if necessary then run:
#
#        python setup.py build
#        python setup.py install

from distutils.core import setup, Extension
import sys, os

# use LID on linux?
useLID = False

# linux WII support?
linuxWii = False        # not implemented yet

# ------------------------------------------------------------------------------

# Detect OS
def getPlatform():
    if sys.platform[:5] == 'linux':
        return 'linux'
    elif sys.platform[:3] == 'win':
        return 'win32'
    elif sys.platform[:6] == 'darwin':
        return 'darwin'
    else:
        return 'unsupported'

# get version
VERSION = 'unknown'
try:
    f = open('../../VERSION')
    VERSION = f.readline()
    f.close()
except:
    pass

# ------------------------------------------------------------------------------
# platform independent
defines = [('PYSCLANG', '1')]

sourceFiles = [	'../../Source/common/fftlib.c',
				'../../Source/common/SC_AllocPool.cpp',
				'../../Source/common/SC_DirUtils.cpp',
				'../../Source/common/SC_Sem.cpp',
				'../../Source/common/SC_StringBuffer.cpp',
				'../../Source/common/SC_StringParser.cpp',
				'../../Source/common/scsynthsend.cpp',
				'../../Source/lang/LangSource/AdvancingAllocPool.cpp',
				'../../Source/lang/LangSource/alloca.cpp',			
				'../../Source/lang/LangSource/ByteCodeArray.cpp',
				'../../Source/lang/LangSource/dumpByteCodes.cpp',
				'../../Source/lang/LangSource/DumpParseNode.cpp',
				'../../Source/lang/LangSource/GC.cpp',
				'../../Source/lang/LangSource/InitAlloc.cpp',
				'../../Source/lang/LangSource/PyrFileUtils.cpp',
				'../../Source/lang/LangSource/PyrInterpreter3.cpp',
				'../../Source/lang/LangSource/PyrLexer.cpp',
				'../../Source/lang/LangSource/PyrMathOps.cpp',
				'../../Source/lang/LangSource/PyrMathSupport.cpp',
				'../../Source/lang/LangSource/PyrMessage.cpp',
				'../../Source/lang/LangSource/PyrObject.cpp',
				'../../Source/lang/LangSource/PyrParseNode.cpp',
				'../../Source/lang/LangSource/PyrSignal.cpp',
				'../../Source/lang/LangSource/PyrSymbolTable.cpp',
				'../../Source/lang/LangSource/Samp.cpp',
				'../../Source/lang/LangSource/SC_LanguageClient.cpp',
				'../../Source/lang/LangSource/SC_LibraryConfig.cpp',
				'../../Source/lang/LangSource/SC_TerminalClient.cpp',
				'../../Source/lang/LangSource/SimpleStack.cpp',
				'../../Source/lang/LangSource/VMGlobals.cpp',
				'../../Source/lang/LangSource/Bison/lang11d_tab.cpp',
				'../../Source/lang/LangPrimSource/OSCData.cpp',
				'../../Source/lang/LangPrimSource/PyrArchiver.cpp',
				'../../Source/lang/LangPrimSource/PyrArrayPrimitives.cpp',
				'../../Source/lang/LangPrimSource/PyrBitPrim.cpp',
				'../../Source/lang/LangPrimSource/PyrCharPrim.cpp',
				'../../Source/lang/LangPrimSource/PyrFilePrim.cpp',
				'../../Source/lang/LangPrimSource/PyrListPrim.cpp',
				'../../Source/lang/LangPrimSource/PyrMathPrim.cpp',
				'../../Source/lang/LangPrimSource/PyrPlatformPrim.cpp',
				'../../Source/lang/LangPrimSource/PyrPrimitive.cpp',
				'../../Source/lang/LangPrimSource/PyrSched.cpp',
				'../../Source/lang/LangPrimSource/PyrSignalPrim.cpp',
				'../../Source/lang/LangPrimSource/PyrStringPrim.cpp',
				'../../Source/lang/LangPrimSource/PyrSymbolPrim.cpp',
				'../../Source/lang/LangPrimSource/PyrSerialPrim.cpp',
				'../../Source/lang/LangPrimSource/PyrUnixPrim.cpp',
				'../../Source/lang/LangPrimSource/PyrUStringPrim.cpp',
				'../../Source/lang/LangPrimSource/SC_ComPort.cpp',
				'../../Source/lang/LangPrimSource/SC_Wii.cpp',

                # PySCLang files
				'./pycxx/Src/cxxextensions.c',
				'./pycxx/Src/cxx_extensions.cxx',
				'./pycxx/Src/cxxsupport.cxx',
				'./pycxx/Src/IndirectPythonInterface.cxx',
				'./PySCLang_Module_GUIStuff.cpp',
				'./PySCLang_vpost_stuff.cpp',
				'./PySCLang_Module.cpp'
			  ]

# ------------------------------------------------------------------------------
# platform specific files and defines

# midi
if os.path.exists('/System/Library/Frameworks/CoreMIDI.framework/Headers/CoreMIDI.h'):
    sourceFiles += ['../../Source/lang/LangPrimSource/SC_CoreMIDI.cpp']
else:
    sourceFiles += ['../../Source/lang/LangPrimSource/SC_AlsaMIDI.cpp']

# Linux
if getPlatform() == 'linux':
    defines += [('SC_LINUX', '1')]

    if useLID:
        defines += [('HAVE_LID', '1')]
        sourceFiles += ['../../Source/lang/LangPrimSource/SC_LID.cpp']

    if linuxWii:
        pass    # todo: add linux wii support

# Mac OS
elif getPlatform() == 'darwin':
    defines += [('SC_DARWIN', '1'), ('HAVE_SPEECH', '1')]

    sourceFiles += ['../../Source/lang/LangPrimSource/SC_Speech.cpp',
                    '../../Source/lang/LangPrimSource/SC_HID.cpp',
                    '../../Source/lang/LangPrimSource/HID_Utilities/HID_Error_Handler.c',
                    '../../Source/lang/LangPrimSource/HID_Utilities/HID_Name_Lookup.c',
                    '../../Source/lang/LangPrimSource/HID_Utilities/HID_Queue_Utilities.c',
                    '../../Source/lang/LangPrimSource/HID_Utilities/HID_Utilities.c',
                    '../../Source/lang/LangPrimSource/WiiMote_OSX/wiiremote.c'
                   ]

# Windows
elif getPlatform() == 'win':
    defines += [('SC_WIN32', '1')]

# ------------------------------------------------------------------------------
# PySCLang module
pysclang = Extension('PySCLang',
                    define_macros = defines,
                    include_dirs = ['../../Headers/app',
									'../../Headers/common',
									'../../Headers/lang',
									'../../Headers/plugin_interface',
									'../../Headers/server',
									'../../Source/lang/LangSource/Bison',
									'./pycxx'
								   ],
                    libraries = ['sndfile', 'scsynth', 'sclang'],
                    library_dirs = ['../../build'],
                    sources = sourceFiles)

setup (name = 'PySCLang',
       version = VERSION,
       description = 'PySCLang module, the python wrapper around the SuperCollider3 sclang program',
       ext_modules = [pysclang])

