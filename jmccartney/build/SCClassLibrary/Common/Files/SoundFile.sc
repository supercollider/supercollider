{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red191\green0\blue0;\red0\green0\blue0;\red0\green0\blue191;
\red0\green115\blue0;}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf2 /*\
	Sound File Format symbols:\
		header formats:\
			read/write formats:\
				'AIFF', 	- Apple's AIFF\
				'WAV','RIFF' 	- Microsoft .WAV\
				'SD2', 	- Sound Designer 2\
				'Sun', 	- NeXT/Sun\
				'IRCAM', 	- old IRCAM format\
				'none'	- no header = raw data\
			A huge number of other formats are supported read only.\
			\
		sample formats:\
			'int8', 'int16', 'int24', 'int32'\
			'mulaw', 'alaw',\
			'float32'\
			\
		not all header formats support all sample formats.\
\
*/\cf3 \
\
\cf4 SoundFile\cf3  \{	\
	\cf4 classvar\cf3  <openFiles;\
	\
	\cf4 var\cf3  <>fileptr;\
	\cf4 var\cf3  <>headerFormat = \cf5 'aiff'\cf3 ;\
	\cf4 var\cf3  <>sampleFormat = \cf5 'float32'\cf3 ;\
	\cf4 var\cf3  <numFrames = 0;		\cf2 // number of frames\
\cf3 	\cf4 var\cf3  <>numChannels = 1;	\cf2 // number of channels\
\cf3 	\cf4 var\cf3  <>sampleRate = 44100.0;\
	\
	*closeAll \{\
		if (openFiles.notNil, \{\
			openFiles.copy.do(\{ \cf4 arg\cf3  file; file.close; \});\
		\});\
	\}\
	isOpen \{\
		^fileptr.notNil\
	\}\
\
	openRead \{ \cf4 arg\cf3  pathName; \
		\cf2 // returns true if success, false if file not found or error reading.\
\cf3 		\cf4 _SFOpenRead\cf3 \
		^\cf4 this\cf3 .primitiveFailed;\
	\}\
	readData \{ \cf4 arg\cf3  rawArray;\
		\cf2 // must have called openRead first!\
\cf3 		\cf2 // returns true if success, false if file not found or error reading.\
\cf3 		\cf4 _SFRead\cf3 \
		^\cf4 this\cf3 .primitiveFailed;\
	\}\
	\
	\
	openWrite \{ \cf4 arg\cf3  pathName;\
		\cf2 // write the header\
\cf3 		\cf2 // format written is that indicated in headerFormat and sampleFormat. \
\cf3 		\cf2 // return true if successful, false if not found or error writing.\
\cf3 		\cf4 _SFOpenWrite\cf3 \
		^\cf4 this\cf3 .primitiveFailed;\
	\}\
	writeData \{ \cf4 arg\cf3  rawArray;\
		\cf2 // must have called openWrite first!\
\cf3 		\cf2 // format written is that indicated in sampleFormat. \
\cf3 		\cf2 // return true if successful, false if not found or error writing.\
\cf3 		\cf4 _SFWrite\cf3 \
		^\cf4 this\cf3 .primitiveFailed;\
	\}\
	close \{\
		\cf4 _SFClose\cf3 \
		^\cf4 this\cf3 .primitiveFailed;\
	\}\
\
	seek \{ \cf4 arg\cf3  offset, origin; \
		\cf2 // origin is an integer, one of: \
\cf3 		\cf2 // 0 - from beginning of file\
\cf3 		\cf2 // 1 - from current position\
\cf3 		\cf2 // 2 - from end of file\
\cf3 		\cf4 _SFSeek\cf3 \
		^\cf4 this\cf3 .primitiveFailed;\
	\}\
	\
	duration \{ ^numFrames/sampleRate \}\
\}\
}