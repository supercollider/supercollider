// The parser skips the statement with the error; only its lines stay as written.
f = {
  arg a;
     a =  ;
};
g = {
arg b;
x=b+1;
x.do({ |i| i.postln });
};
