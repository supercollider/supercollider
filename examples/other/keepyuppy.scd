/*
* KEEPY-UPPY
*
* An AUDIO-ONLY game by Dan Stowell <danstowell@users.sourceforge.net> (C) 2005
  *
  *  This program is free software; you can redistribute it and/or modify
  *  it under the terms of the GNU General Public License as published by
  *  the Free Software Foundation; either version 2 of the License, or
  *  (at your option) any later version.
  *
  *  This program is distributed in the hope that it will be useful,
  *  but WITHOUT ANY WARRANTY; without even the implied warranty of
  *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  *  GNU General Public License for more details.
  *
  *  You should have received a copy of the GNU General Public License
  *  along with this program; if not, write to the Free Software
  *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*
* This game is inspired by the work of "One Switch"
* http://www.oneswitch.org.uk/ - developing games and controllers
* to accommodate various types of disability, including...
* audio-only games.
*
* RULES:
*  You are kicking a football into the air. You can hear it fly
*  up into the air and down again, and when it comes down you
*  must press SPACE to kick it up again.
*
*  The ball may move slightly towards the left or right so you
*  may need to move left or right using the cursor keys to
*  keep within range of the ball (wear headphones for best
*  control!)
*
*  The aim is to kick it up as many times as you can.
*
*
*/


s.boot; // First make sure the server is on

// Then send it the SynthDefs we need
(
SynthDef(\kuball, { |amp=0.5, pan=0, freq=100|
  Out.ar(0, Pan2.ar(amp * 2 * (BPF.ar(WhiteNoise.ar, freq, 0.1)*10).clip2 * SinOsc.ar(4.5), pan))
}).send(s);

SynthDef(\gameover, { Out.ar(0, SinOsc.ar(
				Line.kr(200, 100, 1.0, doneAction:0),
				0,
				EnvGen.kr(Env([0.3, 0.2, 0.2, 0], [0.1, 0.8, 0.1]), 1, doneAction:2)
				).dup(2))}).send(s);

SynthDef(\whackit, {Out.ar(0, SinOsc.ar(74, 0.5pi,
		EnvGen.kr(Env.perc(0, 0.05), doneAction:2)).dup(2))}).send(s);

);

// Now define the game (won't run until we run the last line in this file)
(
~game = {

var whacks, xpos, ypos, xvel, yvel, whack, dogravity, framedur, doc, whackdisabled, whackspacing;


whacks = 0;
xpos   = 0;
ypos   = 0;
xvel   = 0;
yvel   = 10;
framedur = 0.1; // The amount of time between two "frames" of calculation
whackdisabled = false;
whackspacing = 0.3; // The amount of time you must wait between whacks

whack = {
  if(ypos < 20,
  {
	if(xpos.abs < 1,
	{
	  if((yvel < 0) && (whackdisabled==false),
	  {
		yvel = rrand(6.0, 14.0);
		ypos = max(0.1, ypos);
		xvel = 0.1.bilinrand;
		Synth(\whackit);
		whackdisabled = true;
		Task({whackspacing.wait; whackdisabled = false}).play;
		whacks = whacks + 1;
	  });
	});
  });
}; // End of whack function

dogravity = {
  yvel = yvel - 1.0;
  ypos = ypos + yvel;
  xpos = xpos + xvel;
};

Task({
	var kuball;

	kuball = Synth(\kuball);

	// Set up keyboard handlers
	{
		doc = Window("Use LEFT and RIGHT arrows and SPACEBAR");
		doc.front;
	}.defer;

	0.1.wait;
	doc.view.keyDownAction_({arg ...args;
	switch(args[3],
	  32   , {whack.value}, // space
	  63234, {xpos = xpos + 0.1}, // left arrow
	  63235, {xpos = xpos - 0.1} // right arrow
	);
	});

	// This is the loop which runs the game
	while({ypos >= 0},
	{
	dogravity.value;
	kuball.set(\amp, 0.5 * (1.0/max(1.0, ypos)),
			   \pan, max(-1.0, min(1.0, xpos)),
			   \freq, 200 + max(0, ypos * 10));
	framedur.wait;
	});

	// Destroy keyboard handlers
	doc.view.keyDownAction_(nil);
	kuball.free;

	// Run the end-of-game feedback etc
	Synth(\gameover);
	1.5.wait;
	AppClock.sched(0.0, {doc.close;});
	switch(whacks,
	 0, {("You didn't hit the ball - sorry").speak;},
	 1, {("You hit the ball once").speak;},
	 2, {("You hit the ball twice").speak;},
	 3, {("You hit the ball 3 times").speak;},
	 4, {("You hit the ball 4 times - well done").speak;},
	 5, {("You hit the ball 5 times - well done").speak;},
	  {("You hit the ball "+whacks+" times - well done indeed!").speak;}
	);

}).play;


}
)
~game.value; // Press Enter to play!
