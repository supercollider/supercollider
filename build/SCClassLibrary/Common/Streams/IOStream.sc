{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue191;\red191\green0\blue0;}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf2 IOStream\cf0  : \cf2 Stream\cf0  \{\
	reset \{ \cf2 this\cf0 .pos = 0; \}\
	skip \{ \cf2 arg\cf0  n;\
		\cf2 this\cf0 .pos = \cf2 this\cf0 .pos + n;\
	\}\
	comma \{ \cf2 this\cf0 .put(\cf2 Char\cf0 .comma);\}\
	space \{ \cf2 this\cf0 .put(\cf2 Char\cf0 .space); \}\
	nl \{ \cf2 this\cf0 .put(\cf2 Char\cf0 .nl); \}\
	ff \{ \cf2 this\cf0 .put(\cf2 Char\cf0 .ff); \}\
	tab \{ \cf2 this\cf0 .put(\cf2 Char\cf0 .tab); \}\
\
	<< \{ \cf2 arg\cf0  item;\
		item.printOn(\cf2 this\cf0 ); \
	\}\
	<<< \{ \cf2 arg\cf0  item;\
		item.storeOn(\cf2 this\cf0 ); \
	\}\
	<<* \{ \cf2 arg\cf0  collection;\
		collection.printItemsOn(\cf2 this\cf0 ); \
	\}\
	<<<* \{ \cf2 arg\cf0  collection;\
		collection.storeItemsOn(\cf2 this\cf0 ); \
	\}\
	\
	readUpTo \{ \cf2 arg\cf0  delimiter = $\\f;\
		\cf2 var\cf0  string, char;\
		string = \cf2 String\cf0 .new;\
		while (\{\
			char = \cf2 this\cf0 .next;\
			char.notNil and: \{ char != delimiter \}\
		\},\{\
			string = string.add(char);\
		\});\
		^string\
	\}\
	flush \{\}\
\}\
\
\cf2 CollStream\cf0  : \cf2 IOStream\cf0  \{\
	\cf2 var\cf0  <>collection, <pos = 0;\
	\
	*new \{\
		^\cf2 super\cf0 .new.collection_(\cf2 String\cf0 .new(128))\
	\}\
	*on \{ \cf2 arg\cf0  aCollection; \
		^\cf2 super\cf0 .new.on(aCollection); \
	\}\
	on \{ \cf2 arg\cf0  aCollection;\
		collection = aCollection;\
		\cf2 this\cf0 .reset;\
	\}\
	reset \{ super.reset; collection = collection.extend(0) \}\
\
	\
	\cf3 // seeking\
\cf0 	pos_ \{ \cf2 arg\cf0  toPos;\
		pos = toPos.clip(0, collection.size);\
	\}\
	\
	next \{\
		if (pos >= collection.size, \{\
			^\cf2 nil\cf0 \
		\},\{\
			pos = pos + 1;\
			^collection.at(pos - 1);\
		\})\
	\}\
	nextN \{ \cf2 arg\cf0  n;\
		^collection.species.fill(n, \{ \cf2 this\cf0 .next; \});\
	\}\
	contents \{\
		^collection.copyRange(0, collection.size-1);\
	\}\
\
	put \{ \cf2 arg\cf0  item;\
		\cf3 //_RWStream_Put\
\cf0 		if (pos >= collection.size, \{\
			pos = collection.size + 1;\
			collection = collection.add(item);\
		\},\{\
			collection.put(pos, item);\
			pos = pos + 1;\
		\})\
	\}\
	putAll \{ \cf2 arg\cf0  aCollection;\
		collection = collection.overWrite(aCollection, pos);\
		pos = pos + aCollection.size;\
	\}\
	\
	\
\cf3 //	write \{ arg item; \
//		/* writes any of the following items as binary:\
//			a double float,\
//			a long,\
//			an rgb color,\
//			a char,\
//			the name of a Symbol as chars,\
//			the indexable part of any non-Slot format object,\
//				(i.e. Strings, Int8Arrays, Int16Arrays,\
//				Signals, etc.)\
//		*/\
//		_CollStream_Write \
//		^this.primitiveFailed;\
//	\}\
\cf0 \
\cf3 //	getChar \{ _CollStream_GetChar; ^this.primitiveFailed; \}\
//	getInt8 \{ _CollStream_GetInt8; ^this.primitiveFailed; \}\
//	getInt16 \{ _CollStream_GetInt16; ^this.primitiveFailed; \}\
//	getInt32 \{ _CollStream_GetInt32; ^this.primitiveFailed; \}\
//	getFloat \{ _CollStream_GetFloat; ^this.primitiveFailed; \}\
//	getDouble \{ _CollStream_GetDouble; ^this.primitiveFailed; \}\
//	\
//	putChar \{ arg aChar; _CollStream_PutChar; ^this.primitiveFailed; \}\
//	putInt8 \{ arg anInteger; _CollStream_PutInt8; ^this.primitiveFailed; \}\
//	putInt16 \{ arg anInteger; _CollStream_PutInt16; ^this.primitiveFailed; \}\
//	putInt32 \{ arg anInteger; _CollStream_PutInt32; ^this.primitiveFailed; \}\
//	putFloat \{ arg aFloat; _CollStream_PutFloat; ^this.primitiveFailed; \}\
//	putDouble \{ arg aFloat; _CollStream_PutDouble; ^this.primitiveFailed; \}\
//	putString \{ arg aString; _CollStream_PutString; ^this.primitiveFailed; \}\
\cf0 \
\}\
\
\
\cf2 LimitedWriteStream\cf0  : \cf2 CollStream\cf0  \{\
	\cf2 var\cf0  <>limit, <>limitFunc;\
	\
	atLimit \{ ^pos >= limit \}\
	put \{ \cf2 arg\cf0  item;\
		\cf2 var\cf0  newpos;\
		newpos = pos + 1;\
		if (newpos > limit, \{\
			limitFunc.value;\
			limitFunc = \cf2 nil\cf0 ;\
		\},\{\
			\cf2 super\cf0 .put(item);\
		\});\
	\}\
	putAll \{ \cf2 arg\cf0  aCollection;\
		\cf2 var\cf0  newpos;\
		newpos = pos + aCollection.size;\
		if (newpos > limit, \{\
			aCollection = aCollection.copyFromStart(limit - pos - 1);\
			collection = collection.overWrite(aCollection, pos);\
			pos = limit;\
			limitFunc.value;\
			limitFunc = \cf2 nil\cf0 ;\
		\},\{\
			collection = collection.overWrite(aCollection, pos);\
			pos = newpos;\
		\});\
	\}\
	\
\}\
\
\
\cf2 Post\cf0  \{\
	*flush \{ \cf2 this\cf0 .flushPostBuf \}\
	* << \{ \cf2 arg\cf0  item;\
		item.printOn(\cf2 this\cf0 ); \
	\}\
	* <<< \{ \cf2 arg\cf0  item;\
		item.storeOn(\cf2 this\cf0 ); \
	\}\
	* <<* \{ \cf2 arg\cf0  collection;\
		collection.printItemsOn(\cf2 this\cf0 ); \
	\}\
	* <<<* \{ \cf2 arg\cf0  collection;\
		collection.storeItemsOn(\cf2 this\cf0 ); \
	\}\
	* put \{ \cf2 arg\cf0  item;\
		item.post;\
	\}\
	* putAll \{ \cf2 arg\cf0  aCollection;\
		aCollection.post;\
	\}\
	* comma \{ \cf2 this\cf0 .put(\cf2 Char\cf0 .comma);\}\
	* space \{ \cf2 this\cf0 .put(\cf2 Char\cf0 .space); \}\
	* nl \{ \cf2 this\cf0 .put(\cf2 Char\cf0 .nl); \}\
	* ff \{ \cf2 this\cf0 .put(\cf2 Char\cf0 .ff); \}\
	* tab \{ \cf2 this\cf0 .put(\cf2 Char\cf0 .tab); \}\
	* close \{ \cf2 this\cf0 .flush; \}\
\}\
\
\
\cf2 Pretty\cf0  : \cf2 IOStream\cf0  \{ \
	\cf2 var\cf0  <>out, <>level = 0, <>state;\
	*new \{ \cf2 arg\cf0  out;\
		\cf2 var\cf0  stream;\
		stream = \cf2 super\cf0 .new.out_(out);\
		stream.state_(\cf2 PrettyEcho\cf0 (stream));\
		^stream\
	\}\
	put \{ \cf2 arg\cf0  char;\
		state.put(char);\
	\}\
	close \{ out.close; \}\
\}\
\
\
\cf2 PrettyState\cf0  \{\
	\cf2 var\cf0  <>pretty;\
	*new \{ \cf2 arg\cf0  pretty;\
		^\cf2 super\cf0 .new.pretty_(pretty);\
	\}\
\}\
\
\cf2 PrettyEcho\cf0  : \cf2 PrettyState\cf0 \{\
	put \{ \cf2 arg\cf0  char; \
		\cf3 // echo chars until new line\
\cf0 		if ((char == $\\n) || (char == $\\r), \{\
			pretty.out.put($\\n);\
			pretty.state_(\cf2 PrettyEat\cf0 (pretty));\
		\},\{\
			if (char == $\{ , \cf3 /* \} */\cf0  \{\
				pretty.out.put($\\n);\
				pretty.level.do(\{ pretty.out.put($\\t) \});\
				pretty.out.put(char);\
				pretty.out.put($\\n);\
				pretty.level = pretty.level + 1;\
				pretty.state_(\cf2 PrettyEat\cf0 (pretty));\
			\},\{\
				if ( \cf3 /*\{*/\cf0  char == $\}, \{\
					pretty.level = pretty.level - 1;\
					pretty.out.put($\\n);\
					pretty.level.do(\{ pretty.out.put($\\t) \});\
					pretty.out.put(char);\
					pretty.out.put($\\n);\
					pretty.state_(\cf2 PrettyEat\cf0 (pretty));\
				\},\{\
					pretty.out.put(char);\
				\})			\
			\}) \
		\});\
	\}\
\}\
\
\cf2 PrettyEat\cf0  : \cf2 PrettyState\cf0 \{\
	put \{ \cf2 arg\cf0  char;\
		if (char == $\{, \cf3 /*\}*/\cf0  \{\
			pretty.level.do(\{ pretty.out.put($\\t) \});\
			pretty.out.put(char);\
			pretty.out.put($\\n);\
			pretty.level = pretty.level + 1;\
		\},\{\
			if (((char == $\\n) || (char == $\\r)) && (pretty.level == 0), \{\
				pretty.out.put($\\n);\
			\},\{\
				if (char.isSpace.not, \{\
					if ( \cf3 /*\{*/\cf0  char == $\}, \{\
						pretty.level = pretty.level - 1;\
						pretty.level.do(\{ pretty.out.put($\\t) \});\
						pretty.out.put(char);\
						pretty.out.put($\\n);\
					\},\{\
						pretty.level.do(\{ pretty.out.put($\\t) \});\
						pretty.out.put(char);\
						pretty.state_(\cf2 PrettyEcho\cf0 (pretty));\
					\});\
				\});\
			\});\
		\});\
	\}\
\}}