{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue191;\red191\green0\blue0;\red0\green0\blue191;
\red96\green96\blue96;}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf2 Char\cf0  : \cf2 Magnitude\cf0  \{\
	*new \{ ^\cf2 this\cf0 .shouldNotImplement(\cf2 thisMethod\cf0 ) \}\
	*newClear \{ ^\cf2 this\cf0 .shouldNotImplement(\cf2 thisMethod\cf0 ) \}\
	\cf3 // to create a Char use the Integer methods asAscii or asDigit\cf0 \
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural
\cf0 \
	hash \{ \cf4 _ObjectHash\cf0 ; ^\cf4 this\cf0 .primitiveFailed \}\
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural
\cf0 	\
	ascii \{\
		\cf3 // returns the ascii value of a character as an Integer\
\cf0 		\cf2 _AsciiValue\cf0 \
	\}\
	digit \{\
		\cf3 // returns the digit value of a character as an Integer\
\cf0 		\cf2 _DigitValue\cf0 \
		^\cf2 this\cf0 .primitiveFailed\
	\}\
	asAscii \{ ^\cf2 this\cf0  \}\
	\
	\cf3 // case conversion\
\cf0 	toUpper \{\
		\cf2 _ToUpper\cf0 \
	\}\
	toLower \{\
		\cf2 _ToLower\cf0 \
	\}\
	\
	\cf3 // tests return Boolean:\
\cf0 	isAlpha \{\
		\cf3 // is an alphabetic character\
\cf0 		\cf2 _IsAlpha\cf0 \
	\}\
	isAlphaNum \{\
		\cf3 // is an alphabetic character or decimal digit\
\cf0 		\cf2 _IsAlphaNum\cf0 \
	\}\
	isPrint \{\
		\cf3 // is printable\
\cf0 		\cf2 _IsPrint\cf0 \
	\}\
	isPunct \{\
		\cf3 // is punctuation\
\cf0 		\cf2 _IsPunct\cf0 \
	\}\
	isControl \{\
		\cf3 // is a control character\
\cf0 		\cf2 _IsControl\cf0 \
	\}\
	isSpace \{\
		\cf2 _IsSpace\cf0 \
		\cf3 // is white space\
\cf0 	\}\
	isVowel \{	\
		^\cf5 "AEIOU"\cf0 .includes(\cf2 this\cf0 .toUpper);\
	\}\
	\
	isDecDigit \{\
		\cf3 // is a decimal digit 0-9\
\cf0 		\cf2 _IsDecDigit\cf0 \
	\}\
	\
	< \{ \cf2 arg\cf0  aChar;\
		^\cf2 this\cf0 .ascii < aChar.ascii\
	\}\
	\
	*nl \{ ^$\\n \}\
	*ff \{ ^$\\f \}\
	*tab \{ ^$\\t \}\
	*space \{ ^$  \}\
	*comma \{ ^$\\, \}\
\
	draw \{ \
		\cf2 _String_Draw\cf0 \
		^\cf2 this\cf0 .primitiveFailed\
	\}\
\
	printOn \{ \cf2 arg\cf0  stream;\
		stream.put(\cf2 this\cf0 );\
	\}\
	storeOn \{ \cf2 arg\cf0  stream;\
		stream.putAll(\cf2 this\cf0 .asCompileString);\
	\}\
\}\
\
}