(						//w w w w w w w w w w w  ww
s.waitForBoot{{									
	var c,i=0,j=1,								
		syn={|f,o|SinOsc.ar(o*[1,4],LFTri.ar(f,0,pi))}.play,
		d=Document.allDocuments.choose.front;			
		0.5.wait;									
	loop{										
		d.selectRange(i.div(2)%d.string.size,1);c=d.selectedText[0];
		syn.set(\f,c.ascii.midicps/2,\o,c.ascii/80);	
		if(c.isDecDigit,{d.selectedString_((c.ascii/d.string.size).asString);j=j.neg*(c.digit+1)});
		i=i+j;									
		(1/20).wait								
	}											
}												
.fork(AppClock)									
}												
)


// f0 // March 17th 2007