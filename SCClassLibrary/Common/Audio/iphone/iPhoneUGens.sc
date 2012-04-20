AccelerometerX : UGen
{
	// warp 0 = linear
	// warp 1 = exponential
	*kr {
		arg minval=0, maxval=1, warp=0, lag=0.2;
		if (warp === \linear, { warp = 0 });
		if (warp === \exponential, { warp = 1 });

		^this.multiNew('control', minval, maxval, warp, lag)
	}
}

AccelerometerY : AccelerometerX {}
AccelerometerZ : AccelerometerX {}
