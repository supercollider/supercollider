+ MIDIOut {
		// these are required on linux but not osx
		// so we have stubs to prevent errors running linux code on osx
	connect {}
	disconnect {}
	*connect {}
	*disconnect {}
	*connectByUID {}
	*disconnectByUID {}
}
