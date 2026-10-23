// interpreted-version: 3.16
// An inserted semicolon only separates the regions, so it is a warning, the code isn't poisoned, and the regions keep
// their values.
1 + 2
(3 * 4)
f = { |x| if (x) { 5 } { 6 } }
f.value(true)
