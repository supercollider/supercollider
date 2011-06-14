/**
"MoogFF" - Moog VCF digital implementation.
As described in the paper entitled
"Preserving the Digital Structure of the Moog VCF"
by Federico Fontana
appeared in the Proc. ICMC07, Copenhagen, 25-31 August 2007

Original Java code created by F. Fontana - August 2007
federico.fontana@univr.it

Ported to C++ for SuperCollider by Dan Stowell - August 2007
http://www.mcld.co.uk/
*/

MoogFF : Filter {

	*ar { | in, freq=100, gain=2, reset=0, mul=1, add=0 |
		^this.multiNew('audio', in, freq, gain, reset).madd(mul, add)
	}
	*kr { | in, freq=100, gain=2, reset=0, mul=1, add=0 |
		^this.multiNew('control', in, freq, gain, reset).madd(mul, add)
	}
}
