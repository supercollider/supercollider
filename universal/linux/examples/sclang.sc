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
	"alsa_pcm:capture_1,"
	"alsa_pcm:capture_2"
);
"SC_JACK_DEFAULT_OUTPUTS".setenv(
	"alsa_pcm:playback_1,"
	"alsa_pcm:playback_2,"
	"alsa_pcm:playback_3,"
	"alsa_pcm:playback_4,"
	"alsa_pcm:playback_5"
);

// EOF