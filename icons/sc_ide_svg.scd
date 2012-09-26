//SC radial icon XML(SVG) generator
//Batuhan Bozkurt 2012

(
var cids, fids;
var circDefs, fillDefs, uses, wrapped;
var incre, incre2, decre, phase;
var radFor;
var w;

~circle = {|id, cx, cy, r, rotate| "<circle id=\"%\" cx=\"%\\%\" cy =\"%\\%\" r=\"%\\%\" transform=\"rotate(% 512 512)\"/>".format(id, cx, cy, r, rotate) };
~color = {|shade| "rgb(%,%,%)".format(*((shade * 255).round ! 3)); };
~linGrad = {|id, xy1, xy2, c1, c2| "<linearGradient id=\"%\" x1=\"%\\%\" y1=\"%\\%\" x2=\"%\\%\" y2=\"%\\%\"><stop stop-color=\"%\" offset=\"0\\%\"/><stop stop-color=\"%\" offset=\"100\\%\"/></linearGradient>".format(id, xy1.x, xy1.y, xy2.x, xy2.y, ~color.(c1), ~color.(c2)); };
~use = {|shape, fill| "<use xlink:href=\"#%\" fill=\"url(#%)\"/>".format(shape, fill); };
~wrapAll = {|w, h, defs, draws| "<?xml version=\"1.0\" standalone=\"no\"?><!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\"><svg width=\"%\" height=\"%\" viewBox=\"0 0 % %\" version = \"1.1\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\"><desc>SuperCollider Icon SVG - Batuhan Bozkurt 2012</desc><defs>%</defs>%</svg>".format(w, h, w, h, defs, draws); };


radFor = {|cnt| (90 - (13.5 * cnt)); };
w = 100;
phase = -pi;

incre = -0.95;
incre2 = 4.02;
decre = 59;

cids = 7.collect({|cnt| "circ" ++ cnt; });
fids = 7.collect({|cnt| "fcirc" ++ cnt; });

circDefs = 7.collect({|cnt| ~circle.(cids[cnt], 50, 50, radFor.(cnt) / 2, (54 * cnt) % 360); });
fillDefs = 7.collect({|cnt| ~linGrad.(fids[cnt], 50@0, 50@100, cnt.linlin(0, 6, 0, 0.3), 1); });
uses = 7.collect({|cnt| ~use.(cids[cnt], fids[cnt]); });
wrapped = ~wrapAll.(1024, 1024, circDefs.join ++ fillDefs.join, uses.join);
wrapped.postln;
)