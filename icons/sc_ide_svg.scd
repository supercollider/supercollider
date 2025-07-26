//SC radial icon XML(SVG) generator
//Batuhan Bozkurt 2012

(
var cids, fids;
var circDefs, fillDefs, wrapped;
var radFor;
var canvasSize = 1024, pos = (canvasSize / 2).asInt;

~color = {|shade| "rgb(%,%,%)".format(*((shade * 255).round.asInt ! 3)); };

~linGrad = {|id, xy1, xy2, c1, c2| "
    <linearGradient id=\"%\" x1=\"%\\%\" y1=\"%\\%\" x2=\"%\\%\" y2=\"%\\%\">
        <stop stop-color=\"%\" offset=\"0\\%\"/>
        <stop stop-color=\"%\" offset=\"100\\%\"/>
    </linearGradient>".format(id, xy1.x, xy1.y, xy2.x, xy2.y, ~color.(c1), ~color.(c2)); };

~circle = {|id, r, fill, rotate| "
        <circle id=\"%\" cx=\"0\" cy =\"0\" r=\"%\" fill=\"url(#%)\" transform=\"rotate(%)\"/>".format(id, r, fill, rotate) };

~wrapAll = {|w, h, defs, x, y, draws| "<?xml version=\"1.0\" standalone=\"no\"?>
<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">
<svg viewBox=\"0 0 % %\" version = \"1.1\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\">
    <desc>SuperCollider Icon SVG - Batuhan Bozkurt 2012</desc>%
    <g transform=\"translate(%,%)\">%
    </g>
</svg>".format(w, h, defs, x, y, draws); };

cids = 7.collect({|cnt| "circ" ++ cnt; });
fids = 7.collect({|cnt| "fcirc" ++ cnt; });
radFor = {|cnt| (90 - (13.5 * cnt)) / 100 * canvasSize / 2; };

fillDefs = 7.collect({|cnt| ~linGrad.(fids[cnt], 50@0, 50@100, cnt.linlin(0, 6, 0, 0.3), 1); });
circDefs = 7.collect({|cnt| ~circle.(cids[cnt], radFor.(cnt), fids[cnt], (54 * cnt) % 360); });
wrapped = ~wrapAll.(canvasSize, canvasSize, fillDefs.join, pos, pos, circDefs.join);
wrapped.postln;
)
