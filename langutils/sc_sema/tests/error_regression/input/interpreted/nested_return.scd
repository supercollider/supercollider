// interpreted-version: 3.16
// A function defined in interpreted code can't return either, as its home is the interpreted code.
// expect-error: Return in interpreted code
f = { |x| if (x) { ^1 }; 2 }; f.(true)
