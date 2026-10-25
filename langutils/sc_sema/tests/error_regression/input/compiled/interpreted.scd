// compiled
// Interpreted code's regions are each compiled into their own function, in order. A Routine's function, and a
// qualifier's, are in the function that makes them.
a = 1;
( var b = 2, c = [b]; b + c );
r = { :x, x <- [1, 2], x.odd };
f = { |y = 3| y + 1 };
