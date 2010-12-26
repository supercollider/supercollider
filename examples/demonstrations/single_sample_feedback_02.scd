// some more tests


(
{
var x, y, rate, a, b;
	b = LocalBuf(1);
	rate = MouseX.kr(100, SampleRate.ir);
	a = 0.1;
	x = Dbufrd(b); // read from buffer
	x = x + a;
	x = if(x > 1, Dwhite(-1, 1, inf), x);
	y = Dbufwr(x, b); // write to buffer
	Duty.ar(1 / rate, 0, y) * 0.1.dup;
}.scope;
)


(
{
var x, y, rate, a, mod, b;
	b = LocalBuf(1);
	rate = SampleRate.ir;
	a = 0.1;
	mod = Drand([Dseries(-1, 0.01, 200), Dseries(1, -0.1, 20)], inf);
	x = Dbufrd(b); // read from buffer
	x = x + a;
	x = if(x.abs < 0.1, x * mod, x);
	x = x.wrap2(1.0);
	y = Dbufwr(x, b); // write to buffer
	Duty.ar(1 / rate, 0, y) * 0.1.dup;
}.scope;
)



(
{
var x, y, rate, a, mod, b;
	b = LocalBuf(1);
	rate = SampleRate.ir;
	a = 0.1 * 0.8;
	mod = Drand([Dseries(-1, 0.01, MouseX.kr(0, 200)), Dseries(1, -0.1, MouseY.kr(0, 200))], inf);
	x = Dbufrd(b); // read from buffer
	x = x + a;
	x = if(x.abs < 0.1, x * mod, x);
	x = x.wrap2(1.0);
	y = Dbufwr(x, b); // write to buffer
	Duty.ar(1 / rate, 0, y) * 0.1.dup;
}.scope;
)

// brown noise
(
{
var x, y, rate, a, mod, b;
	b = LocalBuf(1);
	rate = SampleRate.ir;
	a = Dwhite(-1, 1, inf) * 0.2;
	x = Dbufrd(b); // read from buffer
	x = x + a;
	x = x.fold2(1.0);
	y = Dbufwr(x, b); // write to buffer
	Duty.ar(1 / rate, 0, y) * 0.1.dup;
}.scope;
)

// harsher brown noise
(
{
var x, y, rate, a, mod, b;
	b = LocalBuf(1);
	rate = SampleRate.ir;
	a = Dwhite(-1, 1, inf) * Drand([0.2, 0.2, 1, 10], inf);
	x = Dbufrd(b); // read from buffer
	x = x + a;
	x = x.fold2(1.0);
	y = Dbufwr(x, b); // write to buffer
	Duty.ar(1 / rate, 0, y) * 0.1.dup;
}.scope;
)



// other noise
(
{
var b = LocalBuf(1);
var read = { Dbufrd(b) };
var write = { |x| Dbufwr(x, b) };
var x, y, rate, a, mod;
	rate = SampleRate.ir;
	a = Dwhite(-1, 1, inf);
	x = read; // read from buffer
	x = x ** a + 1;
	x = x.fold2(1.0);
	y = write.(x); // write to buffer
	Duty.ar(1 / rate, 0, y) * 0.1.dup;
}.scope;
)
