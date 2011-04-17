Plprand : Pwhite {
	embedInStream { |inval|
		var	localLength = length.value(inval),
			pat = Pwhite(lo, hi, localLength);
		^min(pat, pat).embedInStream(inval)
	}
}

Phprand : Pwhite {
	embedInStream { |inval|
		var	localLength = length.value(inval),
			pat = Pwhite(lo, hi, localLength);
		^max(pat, pat).embedInStream(inval)
	}
}

Pmeanrand : Pwhite {
	embedInStream { |inval|
		var	localLength = length.value(inval),
			pat = Pwhite(lo, hi, localLength);
		^((pat + pat) * 0.5).embedInStream(inval)
	}
}

Pbeta : Pattern {
	var <>lo, <>hi, <>prob1, <>prob2, <>length;

	*new{ arg lo = 0.0, hi = 1.0, prob1 = 1, prob2 = 1, length = inf;
		^super.newCopyArgs(lo, hi, prob1, prob2, length);
	}

	storeArgs { ^[lo, hi, prob1, prob2, length] }

	embedInStream { arg inval;
		var loStr = lo.asStream;
		var hiStr = hi.asStream;
		var prob1Str = prob1.asStream;
		var prob2Str = prob2.asStream;
		var loVal, hiVal;

		length.value(inval).do({
			var sum = 2, temp, rprob1, rprob2;
			rprob1 = prob1Str.next(inval);
			rprob2 = prob2Str.next(inval);
			if(rprob1.isNil or: { rprob2.isNil }) { ^inval };
			rprob1 = rprob1.reciprocal;
			rprob2 = rprob2.reciprocal;
			loVal = loStr.next(inval);
			hiVal = hiStr.next(inval);
			if(loVal.isNil or: { hiVal.isNil }) { ^inval };

			while ({
				temp = 1.0.rand ** rprob1;
				sum = temp + (1.0.rand ** rprob2);
				sum > 1;
			});
			inval = (((temp/sum) * (hiVal - loVal)) + loVal).yield;
		});
		^inval;
	}
}

Pcauchy : Pattern {
	var <>mean, <>spread, <>length;

	*new{arg mean = 0.0, spread = 1.0, length = inf;
		^super.newCopyArgs(mean, spread, length);
	}

	storeArgs{ ^[mean, spread, length] }

	embedInStream { arg inval;
		var meanStr = mean.asStream;
		var spreadStr = spread.asStream;
		var meanVal, spreadVal;
		length.value(inval).do({
			var ran = 0.5;
			meanVal = meanStr.next(inval);
			spreadVal = spreadStr.next(inval);
			if(meanVal.isNil or: { spreadVal.isNil }) { ^inval };
			while({
				ran = 1.0.rand;
				ran == 0.5
			});
			inval = ((spreadVal * (ran * pi).tan) + meanVal).yield;
		});
		^inval;
	}
}

Pgauss : Pattern {
	var <>mean, <>dev, <>length;

	*new{ arg mean = 0.0, dev = 1, length = inf;
		^super.newCopyArgs(mean, dev, length);
	}

	storeArgs{ ^[mean, dev, length] }

	embedInStream{arg inval;
		var meanStr = mean.asStream;
		var devStr = dev.asStream;
		var devVal, meanVal;
		length.value(inval).do({
			devVal = devStr.next(inval);
			meanVal = meanStr.next(inval);
			if(meanVal.isNil or: { devVal.isNil }) { ^inval };
			inval = ((((-2*log(1.0.rand)).sqrt * sin(2pi.rand)) * devVal) + meanVal).yield;
		});
		^inval;
	}
}


Ppoisson : Pattern {
	var <>mean, <>length;
	*new{arg mean = 1, length = inf;
		^super.newCopyArgs(mean, length);
		}
	storeArgs{ ^[mean, length] }
	embedInStream{ arg inval;
		var meanStr = mean.asStream;
		length.value(inval).do({
			var inc, test, temp, meanVal = meanStr.next(inval);
			if(meanVal.isNil) { ^inval };
			inc = 0;
			test = 1.0.rand;
			temp = exp(meanVal.neg);
			while({
				test > temp;
				}, {
				inc = inc + 1;
				test = test * 1.0.rand;
			});
			inval = inc.yield;
		});
		^inval;
	}
}

Pexprand : Pwhite {
	*new { arg lo=0.0001, hi=1.0, length=inf;
		^super.newCopyArgs(lo, hi, length)
	}
	storeArgs { ^[ lo, hi, length ] }
	embedInStream { arg inval;
		var loStr = lo.asStream;
		var hiStr = hi.asStream;
		var hiVal, loVal;
		length.value(inval).do({
			hiVal = hiStr.next(inval);
			loVal = loStr.next(inval);
			if(hiVal.isNil or: { loVal.isNil }) { ^inval };
			inval = exprand(loVal, hiVal).yield;
		});
		^inval;
	}
}
