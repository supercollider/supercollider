// interpreted-version: 3.16
// `( ... )` is a function called straight away, so its value is known when the function's result is.
// expect: -> 6
(var q = 2; q * 3)
