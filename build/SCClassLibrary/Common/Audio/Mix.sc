
Mix {
	*new { arg array;
		var sum;
		array.asArray.do({ arg val, i;
			if (i == 0, { sum = val },{ sum = sum + val });
		});
		^sum
	}
	// support this common idiom 
	*fill { arg n, function;
		var val, sum;
		n.do({ arg i;
			val = function.value(i);
			if (i == 0, { sum = val },{ sum = sum + val });
		});
		^sum;
	}
}

