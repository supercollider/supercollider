
/*

This is still experimental and not fully developed.
Gen is a base class for referentially transparent streams. 
A Gen is an object that contains both the current value in the stream and a 'next' method for generating the next Gen.
Unlike Streams or Routines, a Gen doesn't modify itself when it produces the next item in the stream.
Instead it returns another Gen.
You can also copy and/or archive a Gen at any point along the stream, which isn't possible with Routines.
Unfortunately Gens are slower than Patterns due to more allocation.

The GenStream class defined in Stream.sc can wrap a Gen as a Stream.

*/


Gen : AbstractFunction
{
	var <>current;
	
	*new { arg current;
		^super.newCopyArgs(current)
	}
	genCurrent { ^current }
	genNext { ^this.next }
	next { ^nil }
	value { ^this }
	storeArgs { ^[current] }
	asStream { ^GenStream(this) }
	
	// ops
	drop { arg n=1;
		var gen = this;
		n.do { gen = gen.genNext ?? { ^nil } }
		^gen
	}
	dropWhile { arg function;
		var gen = this;
		while { function.(gen.current) } { gen = gen.genNext ?? { ^nil } };
		^gen
	}
	dropUntil { arg function;
		var gen = this;
		while { function.(gen.current).not } { gen = gen.genNext ?? { ^nil } };
		^gen
	}
	keep { arg n=1;
		^Gkeep(this, n)
	}
	keepWhile { arg function;
		^GkeepWhile(this, function)
	}
	keepUntil { arg function;
		^GkeepUntil(this, function)
	}
	select { arg function;
		^Gselect(this, function)
	}
	collect { arg function;
		^Gcollect(this, function)
	}
	++ { arg that;
		^Gcat([this, that])
	}
	add { arg item;
		^this ++ Gen(item)
	}

	stutter { arg n=2, phase=0; ^GStutter(this, n, phase) }
	clump { arg n=2; ^GClump(this, n) }
	
	// function composition
	composeUnaryOp { arg argSelector;
		^GunaryOp.new(argSelector, this)
	}
	composeBinaryOp { arg argSelector, argStream, adverb;
		if(adverb.isNil) {
			^GbinaryOp.new(argSelector, this, argStream)
		} {
			if (adverb == 'x') {
				^GbinaryOpX.new(argSelector, this, argStream);
			};
		};
		^nil
	}
	reverseComposeBinaryOp { arg argSelector, argStream, adverb;
		if(adverb.isNil) {
			^GbinaryOp.new(argSelector, argStream, this)
		} {
			if (adverb == 'x') {
				^GbinaryOpX.new(argSelector, argStream, this);
			};
		};
		^nil
	}
	composeNAryOp { arg argSelector, anArgList;
		^NAryOpStream.new(argSelector, this, anArgList);
	}
}

Ginf : Gen
{
	next { ^this }
}

Gfunc : Gen
{
	var <>nextFunc;
	*new { arg current, nextFunc;
		^super.newCopyArgs(current, nextFunc)
	}
	next { ^nextFunc.value(current) }
	storeArgs { ^[current, nextFunc] }
}

Gseries : Gen
{
	var <>step, <>n;
	*new { arg start = 0, step = 1, n = inf;
		if (n <= 0) { ^nil }
		^super.newCopyArgs(start, step, n)
	}
	next {
		^this.class.new(current+step, step, n-1)
	}
	storeArgs { ^[current, step, n] }
}

Ggeom : Gen
{
	var <>grow, <>n;
	*new { arg start = 1, grow = 2, n = inf;
		if (n <= 0) { ^nil }
		^super.newCopyArgs(start, grow, n)
	}
	next {
		^this.class.new(current*grow, grow, n-1)
	}
	storeArgs { ^[current, grow, n] }
}


Gfib : Gen
{
	var <>b;
	*new { arg a = 0, b = 1;
		^super.newCopyArgs(a, b)
	}
	next {
		^this.class.new(b, current+b)
	}
	storeArgs { ^[current, b] }
}

Gcyc : Gen
{
	var <>list, <>index, <>step;
	*new { arg list, index=0, step=1;
		^super.newCopyArgs(list @@ index, list, index, step)
	}
	next {
		^this.class.new(list, index+step, step) 
	}
	storeArgs { ^[list, index, step] }
}

Giter : Gen
{
	var <>list, <>index, <>step;
	*new { arg list, index=0, step=1;
		var elem = list @ index;
		^elem !? {
			super.newCopyArgs(elem, list, index, step)
		}
	}
	next {
		^this.class.new(list, index+step, step) 
	}
	storeArgs { ^[list, index, step] }
}

GenFilter : Gen
{
	var <>gen;
	storeArgs { ^[gen] }
}

Greset : GenFilter
{
	var <>reset;
	*new { arg gen, reset;
		if (gen.isNil) { ^nil };
		^super.newCopyArgs(gen.genCurrent, gen, reset ? gen)
	}
	next {
		^this.class.new(gen.genNext, reset)
	}
}

Gcollect : GenFilter
{
	var <>function;
	*new { arg gen, function;
		if (gen.isNil) { ^nil };
		^super.newCopyArgs(function.(gen.genCurrent), gen, function)
	}
	next {
		^this.class.new(gen.genNext, function)
	}
	storeArgs { ^[gen, function] }
}

Gselect : GenFilter
{
	var <>function;
	*new { arg gen, function;
		var nx;
		var cur;
		if (gen.isNil) { ^nil };
		cur = gen.genCurrent;
    		if (function.(cur)) {
    			^super.newCopyArgs(cur, gen, function)
    		}{
			nx = gen.genNext;
			^nx !? { this.new(nx, function) }
    		}
	}
	next {
		^this.class.new(gen.genNext, function)
	}
	storeArgs { ^[gen, function] }
}

GunaryOp : GenFilter
{
	var <>selector;
	*new { arg gen, selector;
		if (gen.isNil) { ^nil };
		^super.newCopyArgs(gen.genCurrent.perform(\selector), gen, selector)
	}
	next {
		^this.class.new(gen.genNext, selector)
	}
	storeArgs { ^[gen, selector] }
}

GbinaryOp : GenFilter
{
	var <>gen2, <>selector;
	*new { arg selector, gen, gen2;
		if (gen.isNil) { ^nil };
		if (gen2.isNil) { ^nil };
		^super.newCopyArgs(gen.genCurrent.perform(\selector, gen2.genCurrent), gen, gen2, selector)
	}
	next {
		^this.class.new(gen.genNext, gen2.genNext, selector)
	}
	storeArgs { ^[gen, gen2, selector] }
}

GbinaryOpX : GenFilter
{
	var <>gen2, <>selector, prResetGen2;
	*new { arg selector, gen, gen2, prResetGen2;
		if (gen.isNil) { ^nil };
		if (gen2.isNil) { ^nil };
		^super.newCopyArgs(gen.genCurrent.perform(\selector, gen2.genCurrent), 
			gen, gen2, selector, prResetGen2 ? gen2)
	}
	next {
		var nx2 = gen2.genNext;
		if (nx2.notNil) {
			^this.class.new(selector, gen, nx2, prResetGen2)
		};
		^this.class.new(selector, gen.genNext, prResetGen2)
	}
	storeArgs { ^[gen, gen2, selector, prResetGen2] }
}

GbinaryOpI : GenFilter
{
	var <>gen2, <>selector, prResetGen1, prResetGen2, haveReset1=false, haveReset2 = false;
	*new { arg gen, gen2, selector, prResetGen1, prResetGen2;
		if (gen.isNil) { ^nil };
		if (gen2.isNil) { ^nil };
		^super.newCopyArgs(gen.genCurrent.perform(\selector, gen2.genCurrent), 
			gen, gen2, selector, prResetGen1 ? gen, prResetGen2 ? gen2)
	}
	next {
		var nx1, nx2;
		nx1 = gen.genNext;
		nx2 = gen2.genNext;
		if (nx2.notNil) {
			^this.class.new(gen, nx2, selector. prResetGen2)
		};
		^this.class.new(gen.genNext, prResetGen2, selector)
	}
	storeArgs { ^[gen, gen2, selector, prResetGen2] }
}

Grepeat : GenFilter
{
	var <>n, prResetGen;
	*new { arg gen, n=inf, prResetGen;
		if (n <= 0) { ^nil };
		^super.newCopyArgs(gen.genCurrent, gen, n, prResetGen ?? { gen })
	}
	next {
		var nx = gen.genNext;
		^if (nx.isNil) {
			this.class.new(prResetGen, n-1, prResetGen)
		}{
			this.class.new(nx, n, prResetGen)
		}
	}	
	storeArgs { ^[gen, n, prResetGen] }
}


Gkeep : GenFilter
{
	var <>n;
	*new { arg gen, n;
		if (n <= 0) { ^nil };
		^super.newCopyArgs(gen.genCurrent, gen, n)
	}
	next {
		var nx = gen.genNext;
		^nx !? { this.class.new(nx, n-1) }
	}	
	storeArgs { ^[gen, n] }
}

GkeepWhile : GenFilter
{
	var <>function;
	*new { arg gen, function;
		if (function.(gen.genCurrent).not) { ^nil };
		^super.newCopyArgs(gen.genCurrent, gen, function)
	}
	next {
		var nx = gen.genNext;
		^nx !? { this.class.new(nx, function) }
	}	
	storeArgs { ^[gen, function] }
}

GkeepUntil : GenFilter
{
	var <>function;
	*new { arg gen, function;
		if (function.(gen.genCurrent)) { ^nil };
		^super.newCopyArgs(gen.genCurrent, gen, function)
	}
	next {
		var nx = gen.genNext;
		^nx !? { this.class.new(nx, function) }
	}	
	storeArgs { ^[gen, function] }
}



Gcat : Gen
{
	var <>gens, phase=0, gen;
	*new { arg gens, phase=0, gen;
		if (phase >= gens.size) { ^nil };
		gen = gen ?? { gens[phase].value };
		^super.newCopyArgs(gen.genCurrent, gens, phase, gen)
	}
	next {
		var nx = gen.genNext;
		if (nx.isNil) {
			^this.class.new(gens, phase+1)
		}
		^this.class.new(gens, phase, nx)
	}
	storeArgs { ^[gens, phase, gen] }
	++ { arg that;
		^this.class.new(gens ++ that, phase, gen)
	}
}



Gstutter : GenFilter
{
	var <>n, <>phase;
	*new { arg gen, n=2, phase=0;
		if (gen.isNil) { ^nil };
		if (n <= 0) { ^nil };
		if (n == 1) { ^gen };
		^super.newCopyArgs(gen.genCurrent, gen, n, phase+1)
	}
	next {
		^if (phase < n) {
			this.class.new(gen, n, phase);
		}{
			this.class.new(gen.genNext, n, 0)
		}
	}
}

Gclump : GenFilter
{
	var <>n;
	*new { arg gen, n=2;
		var clump;
		if (gen.isNil) { ^nil };
		if (n <= 0) { ^nil };
		n.do {
			var item;
			item = gen.genCurrent;
			if (item.isNil) {
				if (clump.isNil) { ^nil }
				^Gen(clump)
			};
			clump = clump.add(item);
			gen = gen.genNext;
		};
		^super.newCopyArgs(clump, gen, n)
	}
	next {
		^this.class.new(gen, n)
	}
}

Gzip : Gen
{
	var <>gens;
	*new { arg gens;
		var curs = gens.collect(_.genCurrent);
		^super.newCopyArgs(curs, gens)
	}
	next {
		var nxgens = gens.collect {|gen|
			var nx = gen.genNext;
			if (nx.isNil) { ^nil };
			nx
		};
		^this.class.new(nxgens)
	}
	storeArgs { ^[gens] }
}

Glace : Gen
{
	var <>gens, <>phase=0;
	*new { arg gens, phase=0;
		if (gens.isNil) { ^nil };
		if (gens.size == 0) { ^nil };
		phase = phase % gens.size;
		^super.newCopyArgs(gens.at(phase).genCurrent, gens, phase)
	}
	next {
		var newgens;
		var phase1 = phase + 1;
		if (phase1 >= gens.size) {
			newgens = gens.copy;
			gens.do {|gen, i|
				var nx = gen.genNext;
				if (nx.isNil) { ^nil };
				newgens[i] = nx;
			};
			^this.class.new(newgens, 0)
		};
		^this.class.new(gens, phase1)
	}
	storeArgs { ^[gens, phase] }
}

Gbind : Gen
{
	var <>protoEvent, <>pairs;
	*new { arg protoEvent ... pairs;
		//var event = gen.genCurrent.copy;
		var event = (parent: protoEvent);
		var endval = pairs.size - 1;
		if (pairs.size.odd, { Error("Pbind should have even number of args.\n").throw; });
		forBy (0, endval, 2) { arg i;
			var name = pairs[i];
			var igen = pairs[i+1];	
			if (name.isSequenceableCollection) {
				igen.genCurrent.do { arg val, i;
					event.put(name[i], val);
				};
			}{
				event.put(name, igen.genCurrent);
			};
		}
		^super.newCopyArgs(event, protoEvent, pairs)
	}
	next {
		var endval = pairs.size - 1;
		pairs = pairs.copy;
		forBy (0, endval, 2) { arg i;
			var name = pairs[i];
			var igen = pairs[i+1];	
			var nx = igen.next;
			if (nx.isNil) { ^nil };
			pairs[i+1] = nx;
		}
		^this.class.new(protoEvent, *pairs)
	}
}


GbindF : GenFilter
{
	var <>pairs;
	*new { arg gen ... pairs;
		var event = gen.genCurrent.copy;
		var endval = pairs.size - 1;
		if (pairs.size.odd, { Error("Pbind should have even number of args.\n").throw; });
		forBy (0, endval, 2) { arg i;
			var name = pairs[i];
			var igen = pairs[i+1];	
			if (name.isSequenceableCollection) {
				igen.genCurrent.do { arg val, i;
					event.put(name[i], val);
				};
			}{
				event.put(name, igen.genCurrent);
			};
		}
		^super.newCopyArgs(event, gen, pairs)
	}
	next {
		var nx = gen.genNext;
		var endval = pairs.size - 1;
		if (nx.isNil) { ^nil };
		pairs = pairs.copy;
		forBy (0, endval, 2) { arg i;
			var name = pairs[i];
			var igen = pairs[i+1];	
			var inx = igen.next;
			if (inx.isNil) { ^nil };
			pairs[i+1] = inx;
		}
		^this.class.new(nx, *pairs)
	}
}

Grng : Gen {
	var s1, s2, s3;
}

Gwhite : Grng {
	var <lo, <hi;
	*new { arg seed, lo, hi;
		seed ?? { Date.seed }
	}
	next {
		
	}
}

Gbrown : Grng {
	var <lo, <hi, <step;
	*new { arg seed, lo, hi, step;
		seed ?? { Date.seed }
	}
	next {
		
	}
}

