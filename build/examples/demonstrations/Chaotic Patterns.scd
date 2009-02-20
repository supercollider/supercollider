// From SC2 distro

// Chaotic Patterns, version 0.1
// Some short examples showing how to create music (?) from 
// chaotic iterated maps and (integrated) ODE's. Most of this 
// is probably old news, but there are some new maps like Tinkerbell
// and Ikeda that are interesting. Most interesting is the ease of doing 
// this in SC2 ! Try them several times
// Staffan Liljegren, 991002, staffan@medialab.ericsson.se


// ======================= Discrete Maps ===================================
// Logistics map with fixed r = 3.9 
(
p = Prout({ 
	var x;
	x=0.5.rand;
	loop({
		// xnew = r * xold * (1.0 - xold)
		x = 3.9 * x * (1.0 - x); 
		x.yield;}); });
// p returns points in 0<x<1 and in b we scale it to degrees from 0 - 7. You
// can try freq values instead, by using \freq instead of \degree 
b = Pbind(\dur, 0.1,\degree, p*7);

b.play;

)

// Logistics map with r = 3.687 and independent streams scaled to two octave 
// frygian scale, amp, legato and pan values)
(

p = Prout({ 
	var x;
	x=1.0.rand;
	60.do({
		x = 3.687 * x * (1.0 - x); x.yield;}); });
b = Pbind(\scale, [0,1,3,5,7,8,10],\amp,p/4,\legato, p/2, \pan, p*2-1,\dur, 0.125, \degree, p*14);

Paddp(\mtranspose, Pseq([0,-2, 2],1),Ppar([Pbindf(b, \stretch, 2.0, \octave, 3),b++b])).play;

)

// Logistics map with changing r values 
// (3.4<r<4.0, with "chaos" entering at 3.57)
(

p = Prout({ var r, x;
		// Initial value
		x=0.5.rand;
		 //     32-cycle, chaos?, ghost of 3-cycle in chaos,  chaos!
		 //r = [3.569,    3.687,  3.8282,                     3.9].choose;
		 r = rrand(3.4,4.0);

		 // Uncomment this if you want to see the current r value
		 //("// "++ r).postln;
		 60.do({x = r * x * (1.0 - x); x.yield;}); });
// scale p to a pentatonic scale
b = Pbind(\scale, [0,3,5,7,9], \legato, p/2,\dur, Pseq([Pshuf([0.1,0.2,0.1],4)],5), \degree, p*12);

Pseq([Ppar([Pset(\octave,3,b),Pset(\octave,4,b)])], inf).play;

)



// Tinkerbell map / attractor scaled to 3 octave lydian
// Good for Glass/Reich/Nyman stuff...or ?
(
// I don't know any good initial conditions, but 0<x0,y0<0.3 seems to work, but
// it escapes to infinity on some occasions. I'll try to fix this

// p plays 40 notes with one randomly chosen c1 value 			 
p = Prout({ 
		var x0, y0, c1, c2= -0.6013, c3=2.0, c4=0.4, x, y;
		//vary -0.7<c1<0.9. c1=0.3 is a limit cycle, c1=0.485 is a 7-period, c1=0.9 is chaotic
		c1 = rrand(-0.7,0.9);
		x0=0.2.rand; y0=0.2.rand; 
		// Uncomment this if you want to see the current c1 value
		//("// "++ c1).postln;
		40.do({
			x = x0.squared - y0.squared + (c1*x0) + (c2*y0); 
			y = (2*x0*y0) + (c3*x0) + (c4*y0);
			x0=x; y0=y;
			// I only use the y value, but feel free to experiment with (tiny) x values
			(y*21).yield;}); });
b = Pbind(\scale, [0,2,4,6,7,9,11], \legato, 0.2,\dur,0.1,\degree, p );

Paddp(\mtranspose, Pshuf([0,-3,-1, 3],2),Pseq([Ppar([b,b])], 2)).play;

)

// Ikeda map / attractor scaled to 2 octave aeolian
(

// p plays 40 notes with one randomly chosen c2 value 			 
p = Prout({ var c2; 
		var x0, y0, c1=0.4, c3=6.0, rho=1.0, tao, x, y;
		//vary 0.0<c2<1.0. c2=0.726 is a 3-period, c2=0.780 is "crisis" c2=0.9 is chaotic
		c2 = rrand(0.3,0.9);
		x0=0.2.rand; y0=0.2.rand;
		// Uncomment this if you want to see the current c2 value
		//("// "++ c2).postln;
		40.do({
			tao = c1 - (c3/(1 + x0.squared + y0.squared));
			x = (c2 * ( (x0*cos(tao)) - (y0*sin(tao)) )) + rho;
			y = (c2 * ( (x0*sin(tao)) + (y0*cos(tao)) ));
			x0=x; y0=y;
			[x.abs/5+0.1,y*14].yield;}); });
b = Pbind(\scale, [0,2,3,5,7,8,10],\dur,0.1,[\legato,\degree], p );

Pseq([b],inf).play;
)

// Henon map / attractor (returns points in -1.5<x<1.5, -0.4<y<0.4, which are then
// used for pan (x values) and degrees from 2 octave dorian (y values)
(
p = Prout({ 
	var x0, y0, x, y;
	x0=0; y0=0;
	loop({
		x = y0 + 1 - (1.4*x0*x0); 
		y = 0.3*x0; 
		x0=x;y0=y;
		[x, y*14].yield;}); });
b = Pbind(\scale, [0,2,3,5,7,9,10], \dur,0.125,[\pan, \degree], p );

Ptpar([0.0,Pbindf(b, \octave, 4,\stretch, 3.0), 4.0,b]).play;

)

// ==================== Continous Flows in next version =================================

// Non-working Lorentz attractor (Euler approximated)
(
p = Prout({ 
	var x0,y0,z0,t,x,y,z;
	t=0.01;
	x0=2.0; y0=3.0; z0=5.0;
	loop({ 
		x = x0 + (t*10*(y0-x0)); 
		y = y0 + (t*((x0*(28-z0))-y0)); 
		z = z0 + (t*((x0*y0)-(8*z0/3))); 
		x0=x; y0=y;z0=z;
		[x,y/10,z].postln.yield;}); });
b = Pbind([\pan,\dur,\degree], p);

b.play;
)

// To Come:
// Roessler attractor. See Rossler.ar for a chaotic noise implementation. 
// Duffing Oscillator
