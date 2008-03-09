# setup.py for use with py2exe to create the python module PySCLang
# Created in March 2008 for SC 3.2 by John Glover (glover.john@gmail.com)

# Usage: python setup.py build
#        python setup.py install

# ToDo:
#    - check for platform, use to define appropriate macros and source files
#    - check for LID on Linux?

from distutils.core import setup, Extension

module = Extension('PySCLang',
                    define_macros = [('SC_LINUX', '1'),
                                     ('PYSCLANG', '1')],
                    include_dirs = ['../../Headers/app', 
									'../../Headers/common', 
									'../../Headers/lang', 
									'../../Headers/plugin_interface', 
									'../../Headers/server',
									'../../Source/lang/LangSource/Bison',
									'./pycxx'
								   ],
                    libraries = ['sndfile', 'scsynth', 'sclang'],
                    library_dirs = ['/usr/lib', '../../build'],
                    sources = [	'../../Source/common/dtoa.c',
								'../../Source/common/fftlib.c',
								'../../Source/common/g_fmt.c',
								'../../Source/common/SC_AllocPool.cpp',
								'../../Source/common/SC_DirUtils.cpp',
								'../../Source/common/SC_Sem.cpp',
								'../../Source/common/SC_StringBuffer.cpp',
								'../../Source/common/SC_StringParser.cpp',
								'../../Source/common/scsynthsend.cpp',
								'../../Source/lang/LangSource/AdvancingAllocPool.cpp',
								'../../Source/lang/LangSource/alloca.cpp',			
								'../../Source/lang/LangSource/ByteCodeArray.cpp',
								#'../../Source/lang/LangSource/cmdLineFuncs.cpp',
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
								'../../Source/lang/LangPrimSource/PyrUnixPrim.cpp',
								'../../Source/lang/LangPrimSource/PyrUStringPrim.cpp',
								'../../Source/lang/LangPrimSource/SC_ComPort.cpp',
								#'../../Source/lang/LangPrimSource/SC_HID.cpp',
								'../../Source/lang/LangPrimSource/SC_Wii.cpp',

                                # ToDo: Check for LID before including
                                '../../Source/lang/LangPrimSource/SC_LID.cpp',

								'./pycxx/Src/cxxextensions.c',
								'./pycxx/Src/cxx_extensions.cxx',
								'./pycxx/Src/cxxsupport.cxx',
								'./pycxx/Src/IndirectPythonInterface.cxx',
								'./PySCLang_Module_GUIStuff.cpp',
								'./PySCLang_vpost_stuff.cpp',
								'./PySCLang_Module.cpp'
							  ])

setup (name = 'PySCLang',
       version = '3.2',
       description = 'PySCLang module, the python wrapper around the SuperCollider3 sclang program',
       ext_modules = [module])

