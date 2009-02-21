// =====================================================================
// file:		sclang.sc
// content:		example sclang startup file
// author:		stefan kersten <steve@k-hornz.de>
// =====================================================================
// NOTE:		this is just an example not needed for
//				regular operation of sclang!
// =====================================================================

// set up Server to use a different executable
Server.program = "/usr/local/music/bin/scsynth";

// same for Score
Score.program = Server.program;

// set some server options for a different setup
#[\internal, \local].do { |s|
	s = Server.perform(s);
	s.options.numInputBusChannels = 2;
	s.options.numOutputBusChannels = 5;
};

// hook up jack ports to audio channels
"SC_JACK_DEFAULT_INPUTS".setenv(
	"system:capture_1,"
	"system:capture_2"
);
"SC_JACK_DEFAULT_OUTPUTS".setenv(
	"system:playback_1,"
	"system:playback_2,"
	"system:playback_3,"
	"system:playback_4,"
	"system:playback_5"
);

// EOF