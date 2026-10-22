// A file whose first line declares arguments or variables is a function, the arguments hide the members.
// expect: no diagnostics
// expect-reads: x -> x = 1
// expect-reads: a -> a
|x = 1| var y = x; a = y; a
