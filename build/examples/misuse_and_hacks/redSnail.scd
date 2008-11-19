
// redsnail by redfrik
(				//   s   s   s   s   s   s 
s.waitForBoot{{
	var d= Document.allDocuments.choose.front,
		b, c, i, j= 1, k= 1, m= 0, n= 0, o= 1, p= 1,
		syn= {|j, k, b, c, a| SinOsc.ar(a, BPF.ar(Saw.ar([j, k], [b, c]), 200, 0.1))}.play;
		0.5.wait;
		i= d.string.size.div(2);
	loop{
		d.selectRange(i-j, k+j);
		b= d.selectedText.first; c= d.selectedText.last;
		if(b==$-, {o= o.neg; n= n+1}); j= j+o;
		if(c==$-, {p= p.neg; m= m+1}); k= k+p;
		if(j<=k.neg, {o= 1}); if(k<=j.neg, {p= 1});
		if(m%4==0, {m= m+1; i= i+1});
		if(n%4==0, {n= n+1; i= i-1});
		if(j-i>=0, {j= i+k; i= (d.string.size-j).div(2); k= d.string.size; o= p.neg; p= -1});
		if(k+i>=d.string.size, {k= (i-j).div(2); i= k; j= k; p= o.neg; o= -1});
		syn.set(\j, j, \k, k, \b, b.ascii, \c, c.ascii, \a, j+k/d.string.size);
		(1/c.ascii).wait
	}
}
.fork(AppClock)
}
)				//   s   s   s   s   s   s

