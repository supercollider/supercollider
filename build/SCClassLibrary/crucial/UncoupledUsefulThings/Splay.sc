 
Splay { // spreads each item out evenly across stereo pan

	*arFill { arg n,function;
		var v;
		v = n.reciprocal * 2;
		^Mix.arFill(n.asInteger,{arg i;
			LinPan2.ar(
				function.value(i), 	
				//( i * (2/n) - (n - 1 * (2/n)  / 2) )
				(2 * i - n + 1) / n // thanks jr !
			);
		}) * v
	}
	
	*ar { arg ugenArray;
		var n,v;
		n=ugenArray.size;
		v = n.reciprocal;
		^Mix.arFill(n,{ arg i;
			LinPan2.ar(
				ugenArray.at(i), 
				(2 * i - n + 1) / n
			);
		}) * v
	}
}


// by Jonathan Segel <jsegel@magneticmotorworks.com>
// spreads each item out evenly across multi-channel pan
 
SplayZ : Splay {

	*arFill { arg n, c=4, function;
		var v;
		v = n.reciprocal * 2;
		^Mix.arFill(n.asInteger,{arg i;
					PanAz.ar(c,
						function.value(i),
						(2 * i - n + 1) / n
					);
		}) * v
	}
	
	*ar { arg c=4, ugenArray;
		var n,v;
		n=ugenArray.size;
		v = n.reciprocal;
		^Mix.arFill(n,{ arg i;
			PanAz.ar(c,
				ugenArray.at(i),
				(2 * i - n + 1) / n
			);
		}) * v
	}
}

// adding a spread ratio and center would be cool - cx





