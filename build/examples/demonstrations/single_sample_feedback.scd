


// single sample feedback
// compare LinCongN and Duty

b.free; b = Buffer.alloc(s, 1, 1);

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
	[
		LinCongN.ar(rate, a, c, m), // not exactly the same, but very similar.
		Duty.ar(1 / rate, 0, y)
	] * 0.1;
}.scope;
)

