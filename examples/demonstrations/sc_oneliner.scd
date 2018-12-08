
/*
This one-line example by Lance Putnam is an example of SuperCollider's expressive power. It modulates the pitch of a single sine-wave oscillator.
*/

{SinOsc.ar(OnePole.ar(Mix(LFSaw.ar([1,0.99],[0,0.6],
          2000,2000).trunc([400,600])*[1,-1]),0.98)).dup*0.1}.play
