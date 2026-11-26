(
a = { |x y z| x };
b = { |x, y z| x };
c = { |x = 1 y = 2| x };
d = { |x ...rest| rest }; // the grammar takes no comma before `...`
e = { |x /* one */ y| x };
)
