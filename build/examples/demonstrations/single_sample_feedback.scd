


// single sample feedback

(
Server.internal.waitForBoot {
	b.free; b = Buffer.alloc(Server.internal, 1);
}
);

(
{
var x, y, rate, a, c, m;
	rate = MouseX.kr(100, SampleRate.ir);
	a = 1.1;
	c = 0.13;
	m = 1.0;
	x = Dbufrd(b); // read from buffer
	x = x * a + c % m;
	y = Dbufwr(x, b); // write to buffer
	Duty.ar(1 / rate, 0, y) * 0.1;
}.scope;
)


// compare LinCongN and Duty
// plotting some..


b.free; b = Buffer.alloc(s, 1);
(
{
var x, y, rate, a, c, m, u1, u2;
	rate = SampleRate.ir / 32;
	a = 1.1;
	c = 0.13;
	m = 1.0;
	x = Dbufrd(b); // read from buffer
	x = x * a + c % m;
	y = Dbufwr(x, b); // write to buffer
	u1 = LinCongN.ar(rate, a, c, m);
	u2 = Duty.ar(1 / rate, 0, y);
	
	// not exactly the same, but very similar. lincong uses doubles
	[u1, u2]; 
}.plot(0.1);
)


// put in a delay line and some amplitude modulation
(
{
var x, y, rate, a, c, m, z;
	rate = MouseX.kr(100, SampleRate.ir);
	a = 1.1;
	c = 0.13;
	m = 1.0;
	x = Dbufrd(b); // read from buffer
	x = x * a + c % m;
	z = Duty.ar(1 / rate, 0, Dbufwr(x, b));// write to buffer
	
	// insert a delay and some amplitude modulation
	z = LFSaw.ar(MouseY.kr(1, 10000, 1)) * z;
	z = DelayL.ar(z, 1, 1);
	
	z = Duty.ar(1 / rate, 0, Dbufwr(z, b));// write to buffer
	z * 0.1;
}.scope;
)


