/*
Categorisation of SC UGens, created by Dan Stowell, 2007.
*/
+ A2K             { *categories { ^ #["UGens>Conversion"] } }
+ AbstractIn      { *categories { ^ #["UGens>InOut"] } }
+ AbstractOut     { *categories { ^ #["UGens>InOut"] } }
+ AmpComp         { *categories { ^ #["UGens>Analysis>Amplitude"] } }
+ AmpCompA        { *categories { ^ #["UGens>Analysis>Amplitude"] } } 
+ Amplitude       { *categories { ^ #["UGens>Analysis>Amplitude"] } }
+ AudioIn         { *categories { ^ #["UGens>InOut"] } }
+ Ball            { *categories { ^ #["UGens>Filters>Nonlinear"] } }
+ BasicOpUGen     { }
+ BeatTrack       { *categories { ^ #["UGens>Analysis", "UGens>FFT"] } }
+ BeatTrack2      { *categories { ^ #["UGens>Analysis", "UGens>FFT"] } }
+ BiPanB2         { *categories { ^ #["UGens>Multichannel>Ambisonics"] } }
+ Blip            { *categories { ^ #["UGens>Generators>Deterministic"] } }
+ BufCombN        { *categories { ^ #["UGens>Delays"] } }
+ BufDelayN       { *categories { ^ #["UGens>Delays"] } }
+ BufInfoUGenBase { *categories { ^ #["UGens>Buffer>Info"] } }
+ BufRd           { *categories { ^ #["UGens>Buffer"] } }
+ BufWr           { *categories { ^ #["UGens>Buffer"] } }
+ ChaosGen        { *categories { ^ #["UGens>Generators>Chaotic"] } }
+ CheckBadValues  { *categories { ^ #["UGens>Info"] } }
+ CoinGate        { *categories { ^ #["UGens>Generators>Stochastic"] } }
+ CombN           { *categories { ^ #["UGens>Delays"] } }
+ Compander       { *categories { ^ #["UGens>Analysis>Amplitude"] } }
+ CompanderD      { *categories { ^ #["UGens>Analysis>Amplitude"] } }
+ Convolution     { *categories { ^ #["UGens>FFT"] } }
+ Convolution2    { *categories { ^ #["UGens>FFT"] } }
+ Convolution2L   { *categories { ^ #["UGens>FFT"] } }
+ StereoConvolution2L   { *categories { ^ #["UGens>FFT"] } }
+ Convolution3    { *categories { ^ #["UGens>FFT"] } }
+ COsc            { *categories { ^ #["UGens>Generators>Deterministic"] } }
+ Crackle         { *categories { ^ #["UGens>Generators>Stochastic"] } }
+ DecodeB2		{ *categories { ^ #["UGens>Multichannel>Ambisonics"] } }
+ DUGen			{ *categories { ^ #["UGens>Demand"] } }
+ DC              { *categories { ^ #["UGens>Generators>Single-value"] } }
+ DegreeToKey     { *categories { ^ #["UGens>Conversion"] } }
+ Delay1          { *categories { ^ #["UGens>Delays"] } }
+ DelayN          { *categories { ^ #["UGens>Delays"] } }
+ Demand          { *categories { ^ #["UGens>Demand"] } }
+ DemandEnvGen    { *categories { ^ #["UGens>Demand", "UGens>Envelopes"] } }
+ DetectSilence   { *categories { ^ #["UGens>Synth control", "UGens>Analysis>Amplitude"] } }
+ DiskIn          { *categories { ^ #["UGens>InOut", "UGens>Buffer"] } }
+ DiskOut         { *categories { ^ #["UGens>InOut", "UGens>Buffer"] } }
+ Done            { *categories { ^ #["UGens>Synth control"] } }
+ Dust            { *categories { ^ #["UGens>Generators>Stochastic"] } }
+ Dust2           { *categories { ^ #["UGens>Generators>Stochastic"] } }
+ Duty            { *categories { ^ #["UGens>Demand"] } }
+ Dwhite          { *categories { ^ #["UGens>Demand"] } }
+ DynKlang        { *categories { ^ #["UGens>Generators>Deterministic"] } }
+ DynKlank        { *categories { ^ #["UGens>Generators>Deterministic", "UGens>Filters>Linear"] } }
+ Env             {  }
+ EnvGen          { *categories { ^ #["UGens>Envelopes"] } }
+ ExpRand         { *categories { ^ #["UGens>Generators>Stochastic", "UGens>Generators>Single-value"]} }
+ FFT             { *categories { ^ #["UGens>FFT"] } }
+ FFTTrigger	    { *categories { ^ #["UGens>FFT"] } }
+ Filter          { *categories { ^ #["UGens>Filters>Linear"] } }
+ Formant         { *categories { ^ #["UGens>Generators>Deterministic"] } }
+ Free            { *categories { ^ #["UGens>Synth control"] } }
+ FreeSelf        { *categories { ^ #["UGens>Synth control"] } }
+ FreeSelfWhenDone{ *categories { ^ #["UGens>Synth control"] } }
+ FreeVerb        { *categories { ^ #["UGens>Reverbs"] } }
+ FreeVerb2       { *categories { ^ #["UGens>Reverbs"] } }
+ FreqShift       { *categories { ^ #["UGens>Filters>Nonlinear", "UGens>Filters>Pitch"] } }
+ FSinOsc         { *categories { ^ #["UGens>Generators>Deterministic"] } }
+ Gendy1          { *categories { ^ #["UGens>Generators>Stochastic"] } }        
+ Gendy2          { *categories { ^ #["UGens>Generators>Stochastic"] } }
+ Gendy3          { *categories { ^ #["UGens>Generators>Stochastic"] } }
+ GrainBuf        { *categories { ^ #["UGens>Buffer", "UGens>Generators>Granular"] } }
+ GrainFM         { *categories { ^ #["UGens>Generators>Granular"] } }
+ GrainSin        { *categories { ^ #["UGens>Generators>Granular"] } }
+ GrainIn         { *categories { ^ #["UGens>Generators>Granular"] } }
+ GVerb           { *categories { ^ #["UGens>Reverbs"] } }
+ Hasher          { *categories { ^ #["UGens>Filters>Nonlinear"] } }
+ Hilbert         { *categories { ^ #["UGens>Filters>Nonlinear"] } }
+ HilbertFIR      { *categories { ^ #["UGens>Filters>Nonlinear"] } }
+ IEnvGen         { *categories { ^ #["UGens>Envelopes"] } }
+ IFFT            { *categories { ^ #["UGens>FFT"] } } 
+ Impulse         { *categories { ^ #["UGens>Generators>Deterministic"] } }
+ Index           { *categories { ^ #["UGens>Buffer"] } }
+ IndexL          { *categories { ^ #["UGens>Generators>Deterministic"] } }
+ InfoUGenBase    { *categories { ^ #["UGens>Info"] } }
+ InRange         { *categories { ^ #["UGens>Maths"] } }
+ InRect          { *categories { ^ #["UGens>Maths"] } }
+ IRand           { *categories { ^ #["UGens>Generators>Stochastic", "UGens>Generators>Single-value"]} }
+ K2A             { *categories { ^ #["UGens>Conversion"] } }
+ KeyTrack        { *categories { ^ #["UGens>Analysis>Pitch"] } }
+ KeyState        { *categories { ^ #["UGens>User interaction"] } }
+ Klang           { *categories { ^ #["UGens>Generators>Deterministic", "UGens>Filters>Linear"] } }
+ Klank           { *categories { ^ #["UGens>Generators>Deterministic", "UGens>Filters>Linear"] } }
+ LastValue       { *categories { ^ #["UGens>Triggers"] } }
+ Latch           { *categories { ^ #["UGens>Triggers"] } }
+ LFNoise0        { *categories { ^ #["UGens>Generators>Stochastic"] } }
+ LFPulse         { *categories { ^ #["UGens>Generators>Deterministic"] } }
+ LFSaw           { *categories { ^ #["UGens>Generators>Deterministic"] } }
+ Line            { *categories { ^ #["UGens>Envelopes"] } }
+ Linen           { *categories { ^ #["UGens>Envelopes"] } }
+ LinExp          { *categories { ^ #["UGens>Maths"] } } 
+ LinLin          { *categories { ^ #["UGens>Maths"] } }
+ LinRand         { *categories { ^ #["UGens>Generators>Stochastic", "UGens>Generators>Single-value"]} }
+ Logistic        { *categories { ^ #["UGens>Generators>Chaotic"] } }
+ Loudness        { *categories { ^ #["UGens>Analysis>Amplitude"] } }
+ MantissaMask    { *categories { ^ #["UGens>Filters>Nonlinear"] } }
+ Median          { *categories { ^ #["UGens>Filters>Nonlinear"] } }
+ MFCC            { *categories { ^ #["UGens>Analysis"] } }
+ Mix {}
+ MostChange      { *categories { ^ #["UGens>Maths"] } }
+ MouseButton     { *categories { ^ #["UGens>User interaction"] } }
+ MouseX          { *categories { ^ #["UGens>User interaction"] } }
+ MulAdd          {}
+ Normalizer      { *categories { ^ #["UGens>Analysis>Amplitude"] } }
+ NRand           { *categories { ^ #["UGens>Generators>Stochastic", "UGens>Generators>Single-value"]} }
+ Onsets          { *categories { ^ #["UGens>Analysis"] } }
+ Osc             { *categories { ^ #["UGens>Generators>Deterministic"] } }
+ OscN            { *categories { ^ #["UGens>Generators>Deterministic"] } }
+ PackFFT		    { *categories { ^ #["UGens>FFT"] } }
+ PanB            { *categories { ^ #["UGens>Multichannel>Ambisonics"] } }
+ PanB2           { *categories { ^ #["UGens>Multichannel>Ambisonics"] } }
+ Panner          { *categories { ^ #["UGens>Multichannel>Panners"] } }
+ Pause           { *categories { ^ #["UGens>Synth control"] } }
+ PauseSelf       { *categories { ^ #["UGens>Synth control"] } }
+ PauseSelfWhenDone {  *categories { ^ #["UGens>Synth control"] } }
+ PeakFollower    { *categories { ^ #["UGens>Analysis>Amplitude"] } }
+ Phasor          { *categories { ^ #["UGens>Triggers", "UGens>Buffer"] } }
+ Pitch           { *categories { ^ #["UGens>Analysis>Pitch", "UGens>MachineListening"] } }
+ PitchShift      { *categories { ^ #["UGens>Filters>Pitch"] } }
+ PlayBuf         { *categories { ^ #["UGens>Buffer"] } }
+ Pluck           { *categories { ^ #["UGens>Generators>Deterministic"] } }
+ PMOsc           { *categories { ^ #["UGens>Generators>Deterministic"] } }
+ Poll            { *categories { ^ #["UGens>Info"] } }
+ PSinGrain       { *categories { ^ #["UGens>Generators>Deterministic"] } }
+ Pulse           { *categories { ^ #["UGens>Generators>Deterministic"] } }
+ PulseCount      { *categories { ^ #["UGens>Triggers"] } }
+ PulseDivider    { *categories { ^ #["UGens>Triggers"] } }
+ PV_BinScramble  { *categories { ^ #["UGens>FFT"] } }
+ PV_BinShift     { *categories { ^ #["UGens>FFT"] } }
+ PV_BinWipe      { *categories { ^ #["UGens>FFT"] } }
+ PV_BrickWall    { *categories { ^ #["UGens>FFT"] } }
+ PV_ChainUGen    { *categories { ^ #["UGens>FFT"] } }
+ PV_ConformalMap { *categories { ^ #["UGens>FFT"] } }
+ PV_Diffuser     { *categories { ^ #["UGens>FFT"] } }
+ PV_HainsworthFoote{ *categories { ^ #["UGens>FFT"] } }
+ PV_JensenAndersen { *categories { ^ #["UGens>FFT"] } }
+ PV_MagAbove     { *categories { ^ #["UGens>FFT"] } }
+ PV_MagDiv       { *categories { ^ #["UGens>FFT"] } }
+ PV_MagFreeze    { *categories { ^ #["UGens>FFT"] } }
+ PV_MagMul       { *categories { ^ #["UGens>FFT"] } }
+ PV_MagSmear     { *categories { ^ #["UGens>FFT"] } }
+ PV_MagSquared   { *categories { ^ #["UGens>FFT"] } }
+ PV_PhaseShift   { *categories { ^ #["UGens>FFT"] } } 
+ PV_RandComb     { *categories { ^ #["UGens>FFT"] } }
+ PV_RandWipe     { *categories { ^ #["UGens>FFT"] } }
+ PV_RectComb     { *categories { ^ #["UGens>FFT"] } }
+ PV_RectComb2    { *categories { ^ #["UGens>FFT"] } }
+ Rand            { *categories { ^ #["UGens>Generators>Stochastic", "UGens>Generators>Single-value"] } }
+ RandID          { *categories { ^ #["UGens>Generators>Stochastic"] } }
+ RandSeed        { *categories { ^ #["UGens>Generators>Stochastic"] } }
+ RecordBuf       { *categories { ^ #["UGens>Buffer"] } }
+ Rotate2         { *categories { ^ #["UGens>Multichannel>Ambisonics","UGens>Multichannel>Panners"] } }
+ RunningSum      { *categories { ^ #["UGens>Maths"] } }
+ Saw             { *categories { ^ #["UGens>Generators>Deterministic"] } }
+ ScopeOut        { *categories { ^ #["UGens>Buffer"] } }
+ Select          { *categories { ^ #["UGens>Multichannel>Select"] } }
+ SelectX         { *categories { ^ #["UGens>Multichannel>Select"] } }
+ SelectXFocus    { *categories { ^ #["UGens>Multichannel>Select"] } }
+ SendTrig        { *categories { ^ #["UGens>Triggers"] } }
+ Silent          { *categories { ^ #["UGens>Generators>Single-value"] } }
+ SinOsc          { *categories { ^ #["UGens>Generators>Deterministic"] } }
+ SinOscFB        { *categories { ^ #["UGens>Generators>Deterministic"] } }
+ Slew            { *categories { ^ #["UGens>Filters>Nonlinear"] } }
+ SpecCentroid    { *categories { ^ #["UGens>Analysis", "UGens>FFT"] } }
+ SpecFlatness    { *categories { ^ #["UGens>Analysis", "UGens>FFT"] } }
+ SpecPcile       { *categories { ^ #["UGens>Analysis", "UGens>FFT"] } }
+ Spring          { *categories { ^ #["UGens>Filters>Nonlinear"] } }
+ Stepper         { *categories { ^ #["UGens>Triggers"] } }
+ Sweep           { *categories { ^ #["UGens>Triggers"] } }
+ SyncSaw         { *categories { ^ #["UGens>Generators>Deterministic"] } }
+ T2A             { *categories { ^ #["UGens>Conversion"] } }
+ T2K             { *categories { ^ #["UGens>Conversion"] } }
+ Tap             { *categories { ^ #["UGens>Buffer"] } }
+ TBall           { *categories { ^ #["UGens>Filters>Nonlinear"] } }
+ TDelay          { *categories { ^ #["UGens>Triggers", "UGens>Delays"] } }
+ TExpRand        { *categories { ^ #["UGens>Generators>Stochastic"] } }
+ TGrains         { *categories { ^ #["UGens>Buffer", "UGens>Generators>Granular"] } }
+ Timer           { *categories { ^ #["UGens>Triggers"] } }
+ TIRand          { *categories { ^ #["UGens>Generators>Stochastic"] } }
+ ToggleFF        { *categories { ^ #["UGens>Triggers"] } }
+ TRand           { *categories { ^ #["UGens>Generators>Stochastic"] } }
+ Trig1           { *categories { ^ #["UGens>Triggers"] } }
+ TWindex         { *categories { ^ #["UGens>Generators>Stochastic"] } }
+ TWChoose        { *categories { ^ #["UGens>Multichannel>Select"] } }
+ UGen            { *categories { ^ #["UGens>Unclassified"] } }
+ UnpackFFT	    { *categories { ^ #["UGens>FFT"] } }
+ Unpack1FFT	    { *categories { ^ #["UGens>FFT"] } }
+ VarSaw          { *categories { ^ #["UGens>Generators>Deterministic"] } }
+ Vibrato         { }
+ VOsc            { *categories { ^ #["UGens>Generators>Deterministic"] } }
+ VOsc3           { *categories { ^ #["UGens>Generators>Deterministic"] } }
+ Warp1           { *categories { ^ #["UGens>Buffer", "UGens>Generators>Granular"] } }
+ WhiteNoise      { *categories { ^ #["UGens>Generators>Stochastic"] } }
+ XFade           { *categories { ^ #["UGens>Multichannel>Select"] } }
+ XLine           { *categories { ^ #["UGens>Envelopes"] } }
+ ZeroCrossing    { *categories { ^ #["UGens>Analysis>Pitch"] } }
