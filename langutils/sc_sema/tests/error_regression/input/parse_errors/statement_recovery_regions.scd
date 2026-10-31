// interpreted-version: 3.16
// An error in a function is recovered from inside it, and the region carries on.
var f = { |x| x + ; x * 2 };
f.value(3)
