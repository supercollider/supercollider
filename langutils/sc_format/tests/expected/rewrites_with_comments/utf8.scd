// Multi-byte text before and inside the code that changes: 2, 3, and 4 bytes.
(
a = { arg x /* é */, y = 4.dbamp /* € */; x };
/* 𝄞 */ b = { |x| if(x, { "é" }, /* € */ { "𝄞" }) };
c = { |x = 1, /* ü */ y = "ü"| x };
d = { "€€€".postln /* 𝄞𝄞 */ };
e = { |x| // ünïcödé
	while(x) { "𝄞" } // €
};
f = { if(c) { "é" } { "€" } /* 𝄞 */ };
);
g = "é"; // ü
h = "€" /* 𝄞 */
