PySCLang - a python module for SuperCollider Lang
Project : Psycollider

by:
Benjamin Golinvaux
benjamin.golinvaux@euresys.com
messenger: bgolinvaux@hotmail.com

currently maintained by:
Christopher Frauenberger
frauenberger@iem.at

-------------------------------------------------------------------------------

PySCLang is a python module that implements the SuperCollider lang and makes it
scriptable from python.

Functions available from python:

sendMain(string)
	string is one of the following commands to the language:
	interpretPrintCmdLine - evaluates the current command line string
	run - runs something (defined in the library, does nothing if you didint override it)
	stop - stops the lang
	openWinCodeFile - opens any code window
	methodTemplates -
	methodReferences - finds references to the method selected

compileLibrary()
	compiles the library

setCmdLine(string)
	sets the current command line to any string that contains usually sc3 code

start()
	start the module

setSCLogSink(window)
	set where the module should post to

compiledOK
	returns a bool whether the library was compiled correctly

Rtf2Ascii
	ugly conversion...

setPyPrOpenWinTextFile(path,startRange,rangeSize)
	a file to open

Example usage in python:

>>> import PySCLang, os
>>> os.chdir("/Applications/SuperCollider3/")
>>> PySCLang.start()

>>> PySCLang.setCmdLine("s = Server.local; s.boot; ")
>>> PySCLang.sendMain('interpretPrintCmdLine')
>>> PySCLang.setCmdLine("s.initTree; s.serverRunning=true")
>>> PySCLang.sendMain('interpretPrintCmdLine')

>>> PySCLang.setCmdLine(" { SinOsc.ar(440, 0, 0.4) }.play(s) ")
>>> PySCLang.sendMain('interpretPrintCmdLine')

-------------------------------------------------------------------------------
Compilation

Windows: PySCLang is part of the Solution, please refer to the Windows documentation

OSX: open the Xcode file and
	* make sure Xcode findes libscsysnth.a (in Frameworks) - if in red navigate to it manually
	* set the active build to "Deployment"

Have fun!
