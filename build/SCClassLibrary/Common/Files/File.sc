{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue191;\red191\green0\blue0;\red96\green96\blue96;
}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf0 \
\cf2 UnixFILE\cf0  : \cf2 IOStream\cf0  \{\
	\cf3 // abstract class. Use File or Pipe.\
\cf0 	\
	\cf2 classvar\cf0  <openFiles;\
	\cf2 var\cf0  fileptr;\
	\cf3 // this only supports file sizes < 2^31 for now\
\cf0 \
	*closeAll \{\
		if (openFiles.notNil, \{\
			openFiles.copy.do(\{ \cf2 arg\cf0  file; file.close; \});\
		\});\
	\}\
	isOpen \{\
		^fileptr.notNil\
	\}\
	length \{ \cf3 // returns the length of the file\
\cf0 		\cf2 _FileLength\cf0 ;\
		^\cf2 this\cf0 .primitiveFailed;\
	\}		\
	pos \{ \cf3 // current file position\
\cf0 		\cf2 _FilePos\cf0  \
		^\cf2 this\cf0 .primitiveFailed;\
	\}\
	seek \{ \cf2 arg\cf0  offset, origin; \
		\cf3 // origin is an integer, one of: \
\cf0 		\cf3 // 0 - from beginning of file\
\cf0 		\cf3 // 1 - from current position\
\cf0 		\cf3 // 2 - from end of file\
\cf0 		\
		\cf2 _FileSeek\cf0 \
		^\cf2 this\cf0 .primitiveFailed;\
	\}\
	pos_ \{ \cf2 arg\cf0  toPos;\
		toPos = toPos.clip(0, \cf2 this\cf0 .length - 1);\
		\cf2 this\cf0 .seek(toPos, 0);\
	\}\
	\
	next \{ ^\cf2 this\cf0 .getChar \}\
	nextN \{ \cf2 arg\cf0  n;\
		^\cf2 String\cf0 .fill(n, \{ \cf2 this\cf0 .next; \});\
	\}\
	contents \{\
		^\cf2 this\cf0 .read(\cf2 String\cf0 .new(\cf2 this\cf0 .length));\
	\}\
	\
	put \{ \cf2 arg\cf0  item; \cf2 this\cf0 .write(item); \}\
	putAll \{ \cf2 arg\cf0  aCollection;\
		if (aCollection.isKindOf(\cf2 RawArray\cf0 ), \{\
			\cf2 this\cf0 .write( aCollection );\
		\},\{\
			aCollection.do(\{ \cf2 arg\cf0  item; \cf2 this\cf0 .write(item) \});\
		\});\
	\}\
\
	write \{ \cf2 arg\cf0  item; \
		\cf3 /* writes any of the following items:\
			a double float,\
			a 32 bit int,\
			a char,\
			the name of a Symbol as chars,\
			RawArrays,\
				(i.e. Strings, Int8Arrays, Int16Arrays,\
				Signals, etc.)\
		*/\cf0 \
		\cf2 _FileWrite\cf0  \
		^\cf2 this\cf0 .primitiveFailed;\
	\}\
	read \{ \cf2 arg\cf0  buffer; \
		\cf3 // buffer should be a RawArray.\
\cf0 		\cf3 // fills the buffer, or as much is available.\
\cf0 		\cf3 // returns bytes read.\
\cf0 		\cf2 _FileReadBytes\cf0 ;\
		^\cf2 this\cf0 .primitiveFailed;\
	\}\
	\
	getLine \{ \cf2 arg\cf0  argString;\
		\cf3 // returns a string up to lesser of next newline \
\cf0 		\cf3 // or length-1 of the argument string\
\cf0 		\cf2 _FileReadLine\cf0 ;\
		^\cf2 this\cf0 .primitiveFailed;\
	\}\
		\
	\cf3 // for more fine grained control these read and write a single\
\cf0 	\cf3 // item of the specified type and size\
\cf0 	getChar \{ \cf2 _FileGetChar\cf0 ; ^\cf2 this\cf0 .primitiveFailed; \}\
	getInt8 \{ \cf2 _FileGetInt8\cf0 ; ^\cf2 this\cf0 .primitiveFailed; \}\
	getInt16 \{ \cf2 _FileGetInt16\cf0 ; ^\cf2 this\cf0 .primitiveFailed; \}\
	getInt32 \{ \cf2 _FileGetInt32\cf0 ; ^\cf2 this\cf0 .primitiveFailed; \}\
	getFloat \{ \cf2 _FileGetFloat\cf0 ; ^\cf2 this\cf0 .primitiveFailed; \}\
	getDouble \{ \cf2 _FileGetDouble\cf0 ; ^\cf2 this\cf0 .primitiveFailed; \}\
	\
	putChar \{ \cf2 arg\cf0  aChar; \cf2 _FilePutChar\cf0 ; ^\cf2 this\cf0 .primitiveFailed; \}\
	putInt8 \{ \cf2 arg\cf0  anInteger; \cf2 _FilePutInt8\cf0 ; ^\cf2 this\cf0 .primitiveFailed; \}\
	putInt16 \{ \cf2 arg\cf0  anInteger; \cf2 _FilePutInt16\cf0 ; ^\cf2 this\cf0 .primitiveFailed; \}\
	putInt32 \{ \cf2 arg\cf0  anInteger; \cf2 _FilePutInt32\cf0 ; ^\cf2 this\cf0 .primitiveFailed; \}\
	putFloat \{ \cf2 arg\cf0  aFloat; \cf2 _FilePutFloat\cf0 ; ^\cf2 this\cf0 .primitiveFailed; \}\
	putDouble \{ \cf2 arg\cf0  aFloat; \cf2 _FilePutDouble\cf0 ; ^\cf2 this\cf0 .primitiveFailed; \}\
	putString \{ \cf2 arg\cf0  aString; \cf2 _FilePutString\cf0 ; ^\cf2 this\cf0 .primitiveFailed; \}\
	putString0 \{ \cf2 arg\cf0  aString;\
		\cf2 this\cf0 .putString(aString);\
		\cf2 this\cf0 .putInt8(0);\
	\}\
	putPascalString \{ \cf2 arg\cf0  aString;\
		this.putInt8(aString.size);\cf2 \
\cf0 		this.putString(aString);\cf2 \
	\cf0 \}\
	\
	\cf3 // PRIVATE\
\cf0 	addOpenFile \{\
		openFiles = openFiles.add(\cf2 this\cf0 );\
	\}\
\}\
\
\cf2 File\cf0  : \cf2 UnixFILE\cf0  \{\
	*openDialog \{ \cf2 arg\cf0  prompt, successFunc, cancelFunc;\
		\cf2 var\cf0  path;\
		path = \cf2 this\cf0 .prOpenDialog(prompt);\
		if (path.notNil, \{ successFunc.value(path) \},\{ cancelFunc.value(path) \});\
	\}\
	*saveDialog \{ \cf2 arg\cf0  prompt, defaultName, successFunc, cancelFunc;\
		\cf2 var\cf0  path;\
		path = \cf2 this\cf0 .prSaveDialog(prompt, defaultName);\
		if (path.notNil, \{ successFunc.value(path) \},\{ cancelFunc.value(path) \});\
	\}\
	\
	*new \{ \cf2 arg\cf0  pathName, mode; \
		^\cf2 super\cf0 .new.open(pathName, mode);\
	\}\
	*delete \{ \cf2 arg\cf0  pathName;\
		\cf2 _FileDelete\cf0 \
		^\cf2 this\cf0 .primitiveFailed\
	\}\
	*exists \{ \cf2 arg\cf0  pathName;\
		\cf2 var\cf0  file;\
		file = \cf2 File\cf0 (pathName,\cf4 "r"\cf0 );\
		if (file.isOpen, \{ file.close; ^\cf2 true\cf0  \});\
		^\cf2 false\cf0 \
	\}\
	open \{ \cf2 arg\cf0  pathName, mode;\
		\cf3 /* open the file. mode is a string passed\
			to fopen, so should be one of:\
			"r","w","a","rb","wb","ab","r+","w+","a+",\
			"rb+","wb+","ab+" \
		*/\cf0 \
		if (\cf2 this\cf0 .prOpen(pathName, mode), \{\
			\cf2 this\cf0 .addOpenFile;\
		\});\
	\}\
	close \{ \cf3 // close the file\
\cf0 		\cf3 // the GC will not call this for you\
\cf0 		\cf2 _FileClose\cf0  \
		^\cf2 this\cf0 .primitiveFailed;\
	\}\
\
	\
	\cf3 // PRIVATE\
\cf0 	prOpen \{ \cf2 arg\cf0  pathName, mode;\
		\cf3 /* open the file. mode is a string passed\
			to fopen, so should be one of:\
			"r","w","a","rb","wb","ab","r+","w+","a+",\
			"rb+","wb+","ab+" \
		*/\cf0 \
		\cf2 _FileOpen\cf0  \
		^\cf2 this\cf0 .primitiveFailed;\
	\}\
\
	\cf3 // returns a string for the file's full path.\
\cf0 	\cf3 // returns nil if canceled.\
\cf0 	*prOpenDialog \{ \cf2 arg\cf0  prompt;\
		\cf2 _File_GetFile\cf0 \
		^\cf2 this\cf0 .primitiveFailed;\
	\}\
	\cf3 // returns a string for the file's full path.\
\cf0 	\cf3 // returns nil if canceled.\
\cf0 	*prSaveDialog \{ \cf2 arg\cf0  prompt, defaultName;\
		\cf2 _File_PutFile\cf0 \
		^\cf2 this\cf0 .primitiveFailed;\
	\}\
\
\}\
\
\
\cf2 Pipe\cf0  : \cf2 UnixFILE\cf0  \{\
	\cf3 // pipe stdin to, or stdout from, a unix shell command.\
\cf0 	*new \{ \cf2 arg\cf0  commandLine, mode; \
		^\cf2 super\cf0 .new.open(commandLine, mode);\
	\}\
	open \{ \cf2 arg\cf0  commandLine, mode;\
		\cf3 /* open the file. mode is a string passed\
			to popen, so should be one of:\
			"r","w" \
		*/\cf0 \
		if (\cf2 this\cf0 .prOpen(commandLine, mode), \{\
			\cf2 this\cf0 .addOpenFile;\
		\});\
	\}\
	\
	close \{ \cf3 // close the file\
\cf0 		\cf3 // the GC will not call this for you\
\cf0 		\cf2 _PipeClose\cf0  \
		^\cf2 this\cf0 .primitiveFailed;\
	\}\
\
	\
	\cf3 // PRIVATE\
\cf0 	prOpen \{ \cf2 arg\cf0  pathName, mode;\
		\cf3 /* open the file. mode is a string passed\
			to popen, so should be one of:\
			"r","w" \
		*/\cf0 \
		\cf2 _PipeOpen\cf0  \
		^\cf2 this\cf0 .primitiveFailed;\
	\}\
\}\
\
\
}