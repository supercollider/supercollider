(
// An empty `arg;` on one line goes, keeping the comments by it.
a = { /* c */ 1 };
b = { /* c */ 1 };
c = { // c
	1
};
d = { // c
	1
};
// Not with a comment or a newline between `arg` and `;`.
e = { arg /* c */; 1 };
f = { arg
	; 1 };
)
