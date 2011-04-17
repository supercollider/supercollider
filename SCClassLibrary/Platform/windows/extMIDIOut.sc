+ MIDIOut {
		// these are required on linux but not windows
		// so we have stubs to prevent errors running linux code on windows
	connect {}
	disconnect {}
	*connect {}
	*disconnect {}
	*connectByUID {}
	*disconnectByUID {}
}
