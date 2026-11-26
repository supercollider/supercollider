(
var a=1+2;
// sc-format off
var table = [
  1,  2,  3,
  4,  5,  6
];
// sc-format on
var b=[ 1,
2 ];
a.do({ arg x; x+1; });
)

(
  /*   sc-format off   */
if(a, { 1 }, { 2 });
  /* sc-format on */
if(a, { 1 }, { 2 });
)

// sc-format on
x=1;
// sc-format off is not a switch
y=2;
// sc-format off
z  =  3;
{ arg q; q;
}
