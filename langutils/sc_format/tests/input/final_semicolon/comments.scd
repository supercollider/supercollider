// Comments by a function's last `;`. The `;` goes, the comments stay.
a = { 1; /* c */ };
b = { 1; // c
};
c = { 1 /* a */; /* b */ };
d = { 1 // a
	; /* b */ };
e = { 1 /* a */ ; // b
};
f = { 1;/* c */};
g = {
	x.postln; // what x is
	x + 1; // the result
	// after the result
};
h = { /* c */ 1; };
i = { 1; /* a */ /* b */ };
// A comment that holds a `;` changes nothing.
j = { 1 /* ; */ };
k = { 1 // ;
};
l = { "; }" };
// Nested functions each lose their `;`, the comments in between stay.
m = { { 1; /* inner */ }; /* outer */ };
n = { x.do { |i| i; // each
}; };
// Not after `var`, comments or not.
o = { var y; /* c */ };
