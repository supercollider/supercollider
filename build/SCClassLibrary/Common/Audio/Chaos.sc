/*
Non-linear Dynamic Oscillators
© Lance Putnam 2004
lance@uwalumni.com

This is a set of iterative functions and differential equations that
are known to exhibit chaotic behavior.  Internal calculations are
done with 64-bit words to ensure greater accuracy.

The name of the function is followed by one of N, L, or C.  These
represent the interpolation method used between function iterations.
	N -> None
	L -> Linear
	C -> Cubic

*/

ChaosOsc : UGen {
}

// General Quadratic Map
QuadN : ChaosOsc {
	classvar <equation = "x1 = a*x0^2 + b*x0 + c";
	*ar { arg freq=22050, a=1, b= -1, c= -0.75, x0=0, mul=1, add=0;
		^this.multiNew('audio', freq, a, b, c, x0).madd(mul, add)
	}
}
QuadL : QuadN {}
QuadC : QuadN {}

// Cusp Map
CuspN : ChaosOsc {
	classvar <equation = "x1 = a - b*sqrt(|x0|)";
	*ar { arg freq=22050, a=1, b=1.9, x0=0, mul=1, add=0;
		^this.multiNew('audio', freq, a, b, x0).madd(mul, add)
	}
}
CuspL : CuspN {}

// Gingerbreadman Map
GbmanN : ChaosOsc {
	classvar <equation = "x1 = 1 - y0 + |x0|\ny1 = x0";
	*ar { arg freq=22050, xi=1.2, yi=2.1, mul=1, add=0;
		^this.multiNew('audio', freq, xi, yi).madd(mul, add)
	}
}
GbmanL : GbmanN {}

// Henon Map
HenonN : ChaosOsc {
	classvar <equation = "x2 = 1 - a*(x1^2) + b*x0";
	*ar { arg freq=22050, a=1.4, b=0.3, x0=0, x1=0, mul=1.0, add=0.0;
		^this.multiNew('audio', freq, a, b, x0, x1).madd(mul, add)
	}
}
HenonL : HenonN {}
HenonC : HenonN {}

// Latoocarfian
LatoocarfianN : ChaosOsc {
	classvar <equation = "x1 = sin(b*y0) + c*sin(b*x0)\ny1 = sin(a*x0) + d*sin(a*y0)";
	*ar { arg freq=22050, a=1, b=3, c=0.5, d=0.5, xi=0.5, yi=0.5, mul=1.0, add=0.0;
		^this.multiNew('audio', freq, a, b, c, d, xi, yi).madd(mul, add)
	}
}
LatoocarfianL : LatoocarfianN {}
LatoocarfianC : LatoocarfianN {}

// Linear Congruential
LinCongN : ChaosOsc {
	classvar <equation = "x1 = ((a * x0) + c) % m";
	*ar { arg freq=22050, a=1.1, c=0.13, m=1.0, xi=0, mul=1.0, add=0.0;
		^this.multiNew('audio', freq, a, c, m, xi).madd(mul, add)
	}
}
LinCongL : LinCongN {}
LinCongC : LinCongN {}

// Standard Map
StandardN : ChaosOsc {
	classvar <equation = "x1 = x0 + y0 + k*sin(x0)\ny1 = y0 + k*sin(x0)";
	*ar { arg freq=22050, k=1.0, x0=0.5, y0=0, mul=1.0, add=0.0;
		^this.multiNew('audio', freq, k, x0, y0).madd(mul, add)
	}
}
StandardL : StandardN {}

// Feedback Sine with Linear Congruential Phase Indexing
FBSineN : ChaosOsc {
	classvar <equation = "x1 = sin(a*y0 + b*x0)\ny1 = c*y0 + d";
	*ar { arg freq=22050, a=2, b=1, c=1, d=0.5, xi=0.1, yi=0.1, mul=1, add=0;
		^this.multiNew('audio',freq,a,b,c,d,xi,yi).madd(mul, add)
	}
}
FBSineL : FBSineN {}
FBSineC : FBSineN {}

// ODEs
// 'h' is integration time-step

// Lorenz Attractor
LorenzL : ChaosOsc {
	classvar <equation = "x' = s*(y-x)\ny' = x*(r-z)-y\nz' = x*y - b*z";
	*ar { arg freq=22050, s=10, r=28, b=2.667, h=0.05, x0=0.1, y0=0, z0=0, mul=1.0, add=0.0;
		^this.multiNew('audio', freq, s, r, b, h, x0, y0, z0).madd(mul, add)
	}	
}