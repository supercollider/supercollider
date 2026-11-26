// With keep, lists ending in a comma stay as written.
Pbind(\instrument, \default,
	\freq, 440,
	\dur, 0.25,);
~list = [1, 2, 3,];
~e = (freq: 440, amp: 0.1,);
