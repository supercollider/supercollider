// See server/plugins/DemoUGens.cpp
UnitCmdDemo : UGen {
	*kr {
		^this.multiNew('audio');
	}
}
