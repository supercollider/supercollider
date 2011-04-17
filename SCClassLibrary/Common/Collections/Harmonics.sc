Harmonics {
	var <>size;

	// Harmonics objects are convenient factories for creating Arrays that
	// are used to fill buffers using the b_gen sine fill commands on the server.

	*new { arg size = 32;
		^super.newCopyArgs(size)
	}

	// decays with frequency
	decay { arg k = 1;
		^Array.fill(size) {|i| 1.0 / ((i+1) ** k) }
	}
	geom { arg k = 1.2;
		^Array.fill(size) {|i| 1.0 / (k ** i) }
	}

	// random values
	rand { arg lo=0.0, hi=1.0;
		^Array.rand(size, lo, hi)
	}
	exprand { arg lo=0.01, hi=1.0;
		^Array.exprand(size, lo, hi)
	}
	linrand { arg lo=0.0, hi=1.0;
		^Array.linrand(size, lo, hi)
	}
	rand2 { arg val=1.0;
		^Array.rand2(size, val)
	}
	coin { arg prob = 0.5;
		^Array.fill(size) { if (prob.coin, 1.0, -1.0) }
	}

	// other useful shapes
	formant { arg center=12, width = 3;
		var start, end;
		start = center - (width/2);
		end = center + (width/2);
		^Array.fill(size) {|i|
			if (i <= start) { 0.0 } {
				if (i >= end) { 0.0 } {
					hanWindow((i - start) / width);
				}
			}
		}
	}
	teeth { arg spacing = 2, start = 0;
		^Array.fill(size) {|i|
			if (i < start) { 0.0 } {
				i = i - start;
				if ((i % spacing) == 0, 1.0, 0.0)
			}
		}
	}
	cutoff { arg n;
		^Array.fill(size) {|i| if (i <= n, 1.0, 0.0) }
	}
	shelf { arg start, end, startLevel = 1.0, endLevel = 0.0;
		^Array.fill(size) {|i|
			if (i <= start) { startLevel } {
				if (i >= end) { endLevel } {
					((i - start) / (end - start)) * (endLevel - startLevel) + startLevel;
				}
			}
		}
	}

	sine { arg wavelength=4, iphase=0.5pi, mul=1.0, add=0.0;
		^Array.fill(size) {|i| sin(2pi/wavelength * i + iphase) * mul + add }
	}
	pulse { arg wavelength=4, iphase=0, duty = 0.5, mul=1.0, add=0.0;
		^Array.fill(size) {|i| if (((i - iphase) % wavelength) < duty, 1.0, 0.0) * mul + add  }
	}

	ramp { arg start=1.0, step;
		step = step ? size.reciprocal;
		^Array.series(size, start, step)
	}

}
