Plprand : Pattern {
	var <>lo, <>hi, <>length;
	*new{arg lo = 0.0, hi = 1.0, length = inf;
		^super.newCopyArgs(lo, hi, length);
		}
	storeArgs { ^[lo, hi, length] }
	embedInStream {arg inval;
		length.do({
			inval = min(rrand(lo, hi), rrand(lo, hi)).yield;
			});
		^inval;
	}
}

Phprand : Pattern {
	var <>lo, <>hi, <>length;
	*new{arg lo = 0.0, hi = 1.0, length = inf;
		^super.newCopyArgs(lo, hi, length);
		}
	storeArgs { ^[lo, hi, length] }
	embedInStream {arg inval;
		length.do({
			inval = max(rrand(lo, hi), rrand(lo, hi)).yield;
			});
		^inval;
	}
}

Pmeanrand : Pattern {
	var <>lo, <>hi, <>length;
	*new{arg lo = 0.0, hi = 1.0, length = inf;
		^super.newCopyArgs(lo, hi, length);
		}
	storeArgs { ^[lo, hi, length] }
	embedInStream {arg inval;
		length.do({
			inval = (rrand(lo, hi) + rrand(lo, hi) * 0.5).yield;
			});
		^inval;
	}
}

Pbeta : Pattern {
	var <>lo, <>hi, <>prob1, <>prob2, <>length;
	*new{arg lo = 0.0, hi = 1.0, prob1 = 1, prob2 = 1, length = inf;
		^super.newCopyArgs(lo, hi, prob1, prob2, length);
		}
	storeArgs { ^[lo, hi, prob1, prob2, length] }
	
	embedInStream {arg inval;
		length.do({
			var sum = 2, temp, rprob1, rprob2;
			rprob1 = prob1.reciprocal;
			rprob2 = prob2.reciprocal;
			while ({
				temp = 1.0.rand ** rprob1;
				sum = temp + (1.0.rand ** rprob2);					sum > 1;	
				});
			inval = ((temp/sum) * (hi - lo) + lo).yield;
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
	embedInStream {arg inval;
		length.do({
			var ran = 0.5;
			while({
				ran = 1.0.rand;
				ran == 0.5
				});
			inval = ((spread * (ran * pi).tan) + mean).yield;
			});
		^inval;
	}
}

Pgauss : Pattern {
	var <>mean, <>dev, <>length;
	*new{arg mean = 0.0, dev = 1, length = inf;
		^super.newCopyArgs(mean, dev, length);
		}
	storeArgs{ ^[mean, dev, length] }
	embedInStream{arg inval;
		length.do({
			inval = ((((-2*log(1.0.rand)).sqrt * sin(2pi.rand)) * dev) + mean).yield;
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
	embedInStream{arg inval;
		length.do({
			var inc, test, temp;
			inc = 0;
			test = 1.0.rand;
			temp = exp(mean.neg);
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
	
Pexprand : Pattern {
	var <>lo, <>hi, <>length;
	*new{arg lo = 0.00001, hi = 1.0, length = inf;
		^super.newCopyArgs(lo, hi, length);
		}
	storeArgs { ^[lo, hi, length] }
	embedInStream {arg inval;
		length.do({
			inval = lo.exprand(hi).yield;
			});
		^inval;
		}
	}
