/*
Categorisation of SC UGens, created by Dan Stowell, 2007.
*/
+ A2K             { }
+ AbstractIn      { *categories { ^ #["UGens>I/O"] } }
+ AbstractOut     { *categories { ^ #["UGens>I/O"] } }
+ AmpComp         { *categories { ^ #["UGens>Analysis>Amplitude"] } }
+ AmpCompA        { *categories { ^ #["UGens>Analysis>Amplitude"] } } 
+ Amplitude       { *categories { ^ #["UGens>Analysis>Amplitude"] } }
+ AudioIn         { *categories { ^ #["UGens>I/O"] } }
+ Ball            { *categories { ^ #["UGens>Filters>Nonlinear"] } }
+ BasicOpUGen     { }
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
+ Convolution3    { *categories { ^ #["UGens>FFT"] } }
+ COsc            { *categories { ^ #["UGens>Generators>Deterministic"] } }
+ Crackle         { *categories { ^ #["UGens>Generators>Stochastic"] } }
+ Dbrown          { *categories { ^ #["UGens>Demand"] } }
+ Dbufrd          { *categories { ^ #["UGens>Demand"] } }
+ DC              { *categories { ^ #["UGens>Generators>Single-value"] } }
+ DegreeToKey     { }
+ Delay1          { *categories { ^ #["UGens>Delays"] } }
+ DelayN          { *categories { ^ #["UGens>Delays"] } }
+ Demand          { *categories { ^ #["UGens>Demand"] } }
+ DemandEnvGen    { *categories { ^ #["UGens>Demand", "UGens>Envelopes"] } }
+ DetectSilence   { *categories { ^ #["UGens>Synth control", "UGens>Analysis>Amplitude"] } }
+ Dgeom           { *categories { ^ #["UGens>Demand"] } }
+ DiskIn          { *categories { ^ #["UGens>I/O", "UGens>Buffer"] } }
+ DiskOut         { *categories { ^ #["UGens>I/O", "UGens>Buffer"] } }
+ Done            { *categories { ^ #["UGens>Synth control"] } }
+ Dseries         { *categories { ^ #["UGens>Demand"] } }
+ Dswitch1        { *categories { ^ #["UGens>Demand"] } }
+ Dust            { *categories { ^ #["UGens>Generators>Stochastic"] } }
+ Dust2           { *categories { ^ #["UGens>Generators>Stochastic"] } }
+ Duty            { *categories { ^ #["UGens>Demand"] } }
+ Dwhite          { *categories { ^ #["UGens>Demand"] } }
+ DynKlank        { *categories { ^ #["UGens>Generators>Deterministic"] } }
+ Env             {  }
+ EnvGen          { *categories { ^ #["UGens>Envelopes"] } }
+ ExpRand         { *categories { ^ #["UGens>Generators>Stochastic", "UGens>Generators>Single-value"]} }
+ FFT             { *categories { ^ #["UGens>FFT"] } } 
+ Filter          { *categories { ^ #["UGens>Filters>Linear"] } }
+ Formant         { *categories { ^ #["UGens>Generators>Deterministic"] } }
+ Free            { *categories { ^ #["UGens>Synth control"] } }
+ FreeSelf        { *categories { ^ #["UGens>Synth control"] } }
+ FreeSelfWhenDone{ *categories { ^ #["UGens>Synth control"] } }
+ FreqShift       { *categories { ^ #["UGens>Filters>Nonlinear", "UGens>Filters>Pitch"] } }
+ FSinOsc         { *categories { ^ #["UGens>Generators>Deterministic"] } }
+ Gendy1          { *categories { ^ #["UGens>Generators>Stochastic"] } }        
+ Gendy2          { *categories { ^ #["UGens>Generators>Stochastic"] } }
+ Gendy3          { *categories { ^ #["UGens>Generators>Stochastic"] } }
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
+ K2A             { }
+ KeyState        { *categories { ^ #["UGens>User interaction"] } }
+ Klang           { *categories { ^ #["UGens>Generators>Deterministic"] } }
+ Klank           { *categories { ^ #["UGens>Generators>Deterministic"] } }
+ LastValue       { *categories { ^ #["UGens>Trigger"] } }
+ Latch           { *categories { ^ #["UGens>Trigger"] } }
+ Latoocarfian    { *categories { ^ #["UGens>Generators>Chaotic"] } }
+ LFNoise0        { *categories { ^ #["UGens>Generators>Stochastic"] } }
+ LFPulse         { *categories { ^ #["UGens>Generators>Deterministic"] } }
+ LFSaw           { *categories { ^ #["UGens>Generators>Deterministic"] } }
+ Line            { *categories { ^ #["UGens>Envelopes"] } }
+ Linen           { *categories { ^ #["UGens>Envelopes"] } }
+ LinExp          { *categories { ^ #["UGens>Maths"] } } 
+ LinLin          { *categories { ^ #["UGens>Maths"] } }
+ LinRand         { *categories { ^ #["UGens>Generators>Stochastic", "UGens>Generators>Single-value"]} }
+ ListDUGen       { *categories { ^ #["UGens>Demand"] } }
+ Logistic        { *categories { ^ #["UGens>Generators>Stochastic"] } }
+ MantissaMask    { *categories { ^ #["UGens>Filters>Nonlinear"] } }
+ Median          { *categories { ^ #["UGens>Filters>Nonlinear"] } }
+ Mix {}
+ MostChange      { *categories { ^ #["UGens>Maths"] } }
+ MouseButton     { *categories { ^ #["UGens>User interaction"] } }
+ MouseX          { *categories { ^ #["UGens>User interaction"] } }
+ MulAdd          {}
+ Normalizer      { *categories { ^ #["UGens>Analysis>Amplitude"] } }
+ NRand           { *categories { ^ #["UGens>Generators>Stochastic", "UGens>Generators>Single-value"]} }
+ Osc             { *categories { ^ #["UGens>Generators>Deterministic"] } }
+ OscN            { *categories { ^ #["UGens>Generators>Deterministic"] } }
+ Panner          { *categories { ^ #["UGens>Multichannel"] } }
+ Pause           { *categories { ^ #["UGens>Synth control"] } }
+ PauseSelf       { *categories { ^ #["UGens>Synth control"] } }
+ PauseSelfWhenDone {  *categories { ^ #["UGens>Synth control"] } }
+ PeakFollower    { *categories { ^ #["UGens>Analysis>Amplitude"] } }
+ Peep            {}
+ Phasor          { *categories { ^ #["UGens>Trigger", "UGens>Buffer"] } }
+ Pitch           { *categories { ^ #["UGens>Analysis>Pitch"] } }
+ PitchShift      { *categories { ^ #["UGens>Filters>Pitch"] } }
+ PlayBuf         { *categories { ^ #["UGens>Buffer"] } }
+ PMOsc          { *categories { ^ #["UGens>Generators>Deterministic"] } }
+ Poll            { *categories { ^ #["UGens>Info"] } }
+ PSinGrain       { *categories { ^ #["UGens>Generators>Deterministic"] } }
+ Pulse           { *categories { ^ #["UGens>Generators>Deterministic"] } }
+ PulseCount      { *categories { ^ #["UGens>Trigger"] } }
+ PulseDivider    { *categories { ^ #["UGens>Trigger"] } }
+ PV_BinScramble  { *categories { ^ #["UGens>FFT"] } }
+ PV_BinShift     { *categories { ^ #["UGens>FFT"] } }
+ PV_BinWipe      { *categories { ^ #["UGens>FFT"] } }
+ PV_BrickWall    { *categories { ^ #["UGens>FFT"] } }
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
+ RunningSum      { *categories { ^ #["UGens>Maths"] } }
+ Saw             { *categories { ^ #["UGens>Generators>Deterministic"] } }
+ ScopeOut        { *categories { ^ #["UGens>Buffer"] } }
+ Select          { *categories { ^ #["UGens>Multichannel"] } }
+ SendTrig        { *categories { ^ #["UGens>Trigger"] } }
+ Silent          { *categories { ^ #["UGens>Generators>Single-value"] } }
+ SinOsc          { *categories { ^ #["UGens>Generators>Deterministic"] } }
+ SinOscFB        { *categories { ^ #["UGens>Generators>Deterministic"] } }
+ Slew            { *categories { ^ #["UGens>Filters>Nonlinear"] } }
+ Spring          { *categories { ^ #["UGens>Filters>Nonlinear"] } }
+ Stepper         { *categories { ^ #["UGens>Trigger"] } }
+ Sweep           { *categories { ^ #["UGens>Trigger"] } }
+ SyncSaw         { *categories { ^ #["UGens>Generators>Deterministic"] } }
+ T2K             { }
+ Tap             { *categories { ^ #["UGens>Buffer"] } }
+ TBall           { *categories { ^ #["UGens>Filters>Nonlinear"] } }
+ TDelay          { *categories { ^ #["UGens>Trigger", "UGens>Delays"] } }
+ TExpRand        { *categories { ^ #["UGens>Generators>Stochastic"] } }
+ TGrains         { *categories { ^ #["UGens>Buffer"] } }
+ Timer           { *categories { ^ #["UGens>Trigger"] } }
+ TIRand          { *categories { ^ #["UGens>Generators>Stochastic"] } }
+ ToggleFF        { *categories { ^ #["UGens>Trigger"] } }
+ TPulse          { *categories { ^ #["UGens>Generators>Deterministic"] } }
+ TRand           { *categories { ^ #["UGens>Generators>Stochastic"] } }
+ Trapezoid       { *categories { ^ #["UGens>Maths"] } }
+ Trig1           { *categories { ^ #["UGens>Trigger"] } }
+ TWindex         { *categories { ^ #["UGens>Generators>Stochastic"] } }
+ UGen            { *categories { ^ #["UGens>Unclassified"] } }
+ VarSaw          { *categories { ^ #["UGens>Generators>Deterministic"] } }
+ Vibrato         { }
+ VOsc            { *categories { ^ #["UGens>Generators>Deterministic"] } }
+ VOsc3           { *categories { ^ #["UGens>Generators>Deterministic"] } }
+ WhiteNoise      { *categories { ^ #["UGens>Generators>Stochastic"] } }
+ XFade           { *categories { ^ #["UGens>Multichannel"] } }
+ XLine           { *categories { ^ #["UGens>Envelopes"] } }
+ ZeroCrossing    { *categories { ^ #["UGens>Analysis>Pitch"] } }
