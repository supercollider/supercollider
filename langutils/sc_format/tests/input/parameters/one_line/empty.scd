(
// An empty `arg;` on one line goes, keeping the comments by it.
a = { /* c */ arg; 1 };
b = { arg; /* c */ 1 };
c = { arg; // c
	1
};
d = { // c
	arg; 1
};
// Not with a comment or a newline between `arg` and `;`.
e = { arg /* c */; 1 };
f = { arg
	; 1 };
)
