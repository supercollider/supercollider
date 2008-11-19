//Sam Pluta - 2007
//A poor simulation of LaMonte's Dream House, found on Church St. in New York
//No matter what you think of this patch, you should go check out the real thing!
//If you want to understand the math, read the actual title of the piece
//This will not be good in headphones - use speakers and walk around the room

(
c=0;
d=IdentitySet.new;
(224..288).do{arg i;
	if((((i*(9/8))/((i*(9/8)).floor))==1.0)&&((i*(9/8))<289),{
		d.add(i);
		d.add((i*(9/8)).asInteger);
		c=c+1;
	});
	if((i.isPrime),{
		d.add(i);
		c=c+1;
	});
	if(i%2==1.0,{
		if(((i/2).asInteger.isPrime),{
			d.add(i);
			c=c+1;
		})
	});
	if(i%4==1.0,{
		if(((i/4).asInteger.isPrime),{
			d.add(i);
			c=c+1;
		})
	});
	if(i%8==1.0,{
		if(((i/8).asInteger.isPrime),{
			d.add(i);
			c=c+1;
		})
	});
};
d=d.asArray;
d.sort;
d=d*10;
d.do{arg item, i;
	{ Out.ar(2.rand, SinOsc.ar(item, 0, 0.006), ((2/d.size)*i-1).postln)  }.play;  //if you have more than 2 speakers, change the 2.rand to n.rand, where n is the number of speakers you have
}
)