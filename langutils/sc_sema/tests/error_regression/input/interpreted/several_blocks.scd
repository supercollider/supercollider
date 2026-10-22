// expect: -> 2
// expect-reads: a -> a = 1; a = 2
(var a = 1; a); (var a = 2; a)
