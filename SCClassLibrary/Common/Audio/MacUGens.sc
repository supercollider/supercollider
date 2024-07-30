MouseX : UGen {
    // Although these touch shared state, it isn't possible to mutate the mouse nor keyboard,
    //   so this isn't considered true 'resource'.
	resourceManagers { ^[] }
	hasObservableEffect { ^false }

	// warp 0 = linear
	// warp 1 = exponential
	*kr {
		arg minval=0, maxval=1, warp=0, lag=0.2;
		if (warp === \linear, { warp = 0 });
		if (warp === \exponential, { warp = 1 });

		^this.multiNew('control', minval, maxval, warp, lag)
	}
	signalRange { ^\unipolar }
}

MouseY : MouseX {}

MouseButton : UGen {
	resourceManagers { ^[] }
	hasObservableEffect { ^false }

	*kr {
		arg minval=0, maxval=1, lag=0.2;
		^this.multiNew('control', minval, maxval, lag)
	}
	signalRange { ^\unipolar }
}

KeyState : UGen {
	resourceManagers { ^[] }
	hasObservableEffect { ^false }

	*kr {
		arg keycode=0, minval=0, maxval=1, lag=0.2;
		^this.multiNew('control', keycode, minval, maxval, lag)
	}
	signalRange { ^\unipolar }
}
