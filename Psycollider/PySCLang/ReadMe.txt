
functions :

sendMain(string)
compileLibrary()
setCmdLine(string)
start()

-------------------------------------------------------------------------------

python session :

import RRRRR as *

s1 = LoadBuffer("machin")
s2 = LoadBuffer("truc")


Play :

(
// read a whole sound into memory
s = Server.local;
// note: not *that* columbia, the first one
b = Buffer.read(s,"sounds/a11wlk01.wav");
)

SynthDef("help-PlayBuf",{ arg out=0,bufnum=0;
	Out.ar(out,
		PlayBuf.ar(1,bufnum, BufRateScale.kr(bufnum))
	)
}).play(s,[\out, 0, \bufnum, b.bufnum]);


