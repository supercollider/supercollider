


// single sample feedback

// a simple sawtooth

(
{
var x, y, rate, a, c, m, b;
	b = LocalBuf(1);
	rate = MouseX.kr(100, SampleRate.ir);
	a = 0.1;
	c = 0.13;
	m = 1.0;
	x = Dbufrd(b); // read from buffer
	x = x + a;
	x = if(x > 1, 0, x);
	y = Dbufwr(x, b); // write to buffer
	Duty.ar(1 / rate, 0, y) * 0.1.dup;
}.scope;
)

// linear congruence

(
{
var x, y, rate, a, c, m, b;
	b = LocalBuf(1);
	rate = MouseX.kr(100, SampleRate.ir);
	a = 1.1;
	c = 0.13;
	m = 1.0;
	x = Dbufrd(b); // read from buffer
	x = x * a + c % m;
	y = Dbufwr(x, b); // write to buffer
	Duty.ar(1 / rate, 0, y) * 0.1.dup;
}.scope;
)


// compare LinCongN and Duty
// plotting some..


(
{
var x, y, rate, a, c, m, u1, u2, b;
	b = LocalBuf(1);
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


// put in a delay line and some amplitude modulation and couple stereo pairs
(
{
var x, y, rate, a, c, m, z, b;
	b = LocalBuf(1);
	rate = MouseX.kr(100, SampleRate.ir) * [1, 1.2];
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
