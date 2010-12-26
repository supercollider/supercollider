(					//z z z z z z z z z z z z z z z z
s.waitForBoot{{									
	var d= Document.allDocuments.choose.front,		
		c, i= 0, j= 1, k= 1,						
		syn= {|f, o| SinOsc.ar(f*[1, 1.01], LPF.ar(LFSaw.ar(f, 0, o), 80))}.play;
		0.5.wait;									
	loop{										
		d.selectRange(i.div(2)%d.string.size, i%2); c= d.selectedText[0];
		syn.set(\f, 99*k+c.ascii, \o, i%(d.string.size*2)/d.string.size*99);
		i= i+j;									
		if("%&@".includes(c), {i= i-[8, 4, -1].choose});
		if("(){}.,:".includes(c), {j= [-1, 1, 0.25, 1/3, 3]@@i});
		if(c==$\n, {k= [1, 1.06, 0.5]@@i});			
		(1/40).wait								
	}											
}												
.fork(AppClock)									
}												
)					//z z z z z z z z z z z z z z z z

// f0 // March 17th 2007