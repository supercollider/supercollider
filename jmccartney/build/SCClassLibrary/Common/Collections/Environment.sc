{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue0;\red0\green0\blue191;\red191\green0\blue0;
\red0\green115\blue0;}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf2 \
\cf3 Environment\cf2  : \cf3 IdentityDictionary\cf2  \{\
\
	*new \{ \cf3 arg\cf2  size=8; ^\cf3 super\cf2 .new(size) \}\
	*make \{ \cf3 arg\cf2  function;\
		^\cf3 this\cf2 .new.make(function)\
	\}\
	*use \{ \cf3 arg\cf2  function;\
		^\cf3 this\cf2 .new.use(function)\
	\}\
\
	make \{ \cf3 arg\cf2  function;\
		\cf4 // pushes the Envir, executes function, returns the Envir\
\cf2 		\cf4 // usually used to create an environment by adding new variables to it.\
\cf2 		\cf3 var\cf2  result, saveEnvir;\
		\
		saveEnvir = currentEnvironment;\
		currentEnvironment = \cf3 this\cf2 ;\
		function.value(\cf3 this\cf2 );\
		currentEnvironment = saveEnvir;\
	\}\
	use \{ \cf3 arg\cf2  function;\
		\cf4 // temporarily replaces the currentEnvironment with this, \
\cf2 		\cf4 // executes function, returns the result of the function\
\cf2 		\cf3 var\cf2  result, saveEnvir;\
		\
		saveEnvir = currentEnvironment;\
		currentEnvironment = \cf3 this\cf2 ;\
		result = function.value(\cf3 this\cf2 );\
		currentEnvironment = saveEnvir;\
		^result\
	\}\
	\
	eventAt \{ \cf3 arg\cf2  key; ^\cf3 this\cf2 .at(key) \}	\
\}\
\
\cf4 // Events are returned by Pattern Streams\
\cf2 \
\cf3 Event\cf2  : \cf3 Environment\cf2  \{\
	\cf3 classvar\cf2  <defaultProtoEvent;\
	\
	delta \{\
		\cf3 _Event_Delta\cf2 \
		^\cf3 this\cf2 .primitiveFailed;\
		\cf4 /*\
		// implemented by primitive for speed\
		var delta;\
		delta = this.at('delta');\
		if (delta.notNil, \{ ^delta \},\{ ^this.at('dur') * this.at('stretch') \});\
		*/\cf2 \
	\}\
	play \{\
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural
\cf2 		\cf3 this\cf2 .at(\cf5 \\player\cf2 ).playEvent(\cf3 this\cf2 );\
		^\cf3 this\cf2 .delta / \cf3 this\cf2 .at(\cf5 \\tempo\cf2 )\
	\}\
	\
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural
\cf2 	*protoEvent \{ ^defaultProtoEvent.copy \}\
	*initClass \{		\
		defaultProtoEvent = \cf3 this\cf2 .make(\{		\
			\cf4 // fill prototype event with default property values\
\cf2 			\
			~reverse = \cf3 false\cf2 ;\
			\
			~tempo = 1;	\cf4 // beats per second\
\cf2 			~dur = 1.0;\
			~stretch = 1.0;\
			~legato = 0.8;\
			~sustain = \{ ~dur * ~legato * ~stretch \};\
			~lag = 0.0;\
			~strum = 0.0;\
			~fxIndex = 0;\
			~monoIndex = 0;\
			\
			~amp = \{ ~db.dbamp \};\
			~db = -20.0;\
			~velocity = 64; 		\cf4 // MIDI units 0-127\
\cf2 			~pan = 0.0; 			\cf4 // pan center\
\cf2 			~channels = 2;\
			\
			~mtranspose = 0;\
			~gtranspose = 0.0;\
			~ctranspose = 0.0;\
			\
			~octave = 5.0;\
			~root = 0.0;		\cf4 // root of the scale\
\cf2 			~degree = 0;\
			~scale = #[0, 2, 4, 5, 7, 9, 11]; \cf4 // diatonic major scale\
\cf2 			~stepsPerOctave = 12.0;\
			~detune   = 0.0;		\cf4 // detune in Hertz\
\cf2 			\
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural
\cf2 			~note = \{\
				(~degree + ~mtranspose).degreeToKey(~scale, ~stepsPerOctave);\
			\};\
			~midinote = \{\
				\cf3 var\cf2  divs;	\
				divs = ~stepsPerOctave;\
				(~note.value + ~gtranspose + (~octave * divs) + ~root)\
					 * 12.0 / divs; \
			\};\
			~freq = \{\
				(~midinote.value + ~ctranspose).midicps;\
			\};\
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural
\cf2 			\
			~env = \cf3 Env\cf2 .asr(0.01, 1.0, 0.5);\
			~envgen = \{ \cf3 EnvGen\cf2 .kr(~env, 1, ~amp) \};\
\
			~chanOffset = 0;\
			~instrument = \cf5 \\default\cf2 ;\
			~wavetable = \cf3 Wavetable\cf2 .sineFill(1024, 1/[1,2,3,4,5,6]);\
			~ugenFunc = \{ \
				\cf3 Library\cf2 .at(\cf5 \\instruments\cf2 , ~instrument).ugenFunc.valueEnvir; \
			\};\
			\
			~argNames = #[\cf5 \\freq\cf2 , \cf5 \\amp\cf2 , \cf5 \\pan\cf2 ];\
			~group = 0;\
			\
			~finish = \{\
				\cf4 // do final calculations\
\cf2 				~freq = ~freq.value + ~detune;\
				~amp = ~amp.value;\
				~sustain = ~sustain.value;\
			\};\
						\
			~player = \cf3 NotePlayer\cf2 .new;\
			\cf0 ~latency = 0.05;\cf2 \
		\});\
	\}\
\}\
\
\
}