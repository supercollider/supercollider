
(
var linetext, drawletter;
var w, h = 800, v = 60, seed = Date.seed, run = true;
var time, name, sourcestring;
var yellowness, penwidth, pen;

//name=[\s,\u,\p,\e,\r,\c,\o,\l,\l,\i,\d,\e,\r];

//sourcestring= "any lower case text";

sourcestring= "welcome to supercollider";

name=Array.fill(sourcestring.size,{arg i; sourcestring[i].asSymbol});

time=0;

linetext= (
'a':[[[0,1],[0.5,0]],[[0.5,0],[1,1]],[[0.25,0.5],[0.75,0.5]]],
'b':[[[0,1],[0,0]],[[0,1],[1,1]],[[0,0],[1,0]],[[0,0.5],[0.75,0.5]],[[0.75,0.5],[1,0.75]],[[0.75,0.5],[1,0.25]],[[1,0.75],[1,1]],[[1,0.25],[1,0]]],
'c':[[[0,1],[0,0]],[[0,0],[1,0]],[[0,1],[1,1]]],
'd':[[[0,1],[0,0]],[[0,0],[0.75,0]],[[0,1],[0.75,1]],[[0.75,1],[1,0.75]],[[0.75,0],[1,0.25]],[[1,0.25],[1,0.75]]],
'e':[[[0,0],[0,1]],[[0,0],[1,0]],[[0,1],[1,1]],[[0,0.5],[1,0.5]]],
'f':[[[0,0],[0,1]],[[0,0],[1,0]],[[0,0.5],[1,0.5]]],
'g':[[[0,1],[0,0]],[[0,0],[1,0]],[[0,1],[1,1]],[[1,1],[1,0.5]],[[0.5,0.5],[1,0.5]]],
'h':[[[0,1],[0,0]],[[0,0.5],[1,0.5]],[[1,1],[1,0]]],
'i':[[[0,0],[1,0]],[[0.5,0],[0.5,1]],[[0,1],[1,1]]],
'j':[[[0,0],[1,0]],[[0.5,0],[0.5,1]],[[0,1],[0.5,1]]],
'k':[[[0,1],[0,0]],[[0,0.5],[1,1]],[[0,0.5],[1,0]]],
'l':[[[0,1],[0,0]],[[0,1],[1,1]]],
'm':[[[0,1],[0,0]],[[0,0],[0.5,0.5]],[[0.5,0.5],[1,0]],[[1,0],[1,1]]],
'n':[[[0,1],[0,0]],[[0,0],[1,1]],[[1,1],[1,0]]],
'o':[[[0,1],[0,0]],[[0,0],[1,0]],[[0,1],[1,1]],[[1,0],[1,1]]],
'p':[[[0,0],[0,1]],[[0,0],[1,0]],[[0,0.5],[1,0.5]],[[1,0],[1,0.5]]],
'q':[[[0,0],[0,0.75]],[[0,0],[0.75,0]],[[0,0.75],[0.75,0.75]],[[0.75,0],[0.75,0.75]],[[0.5,0.5],[1,1]]],
'r':[[[0,0],[0,1]],[[0,0],[1,0]],[[0,0.5],[1,0.5]],[[1,0],[1,0.5]],[[0,0.5],[1,1]]],
's':[[[0,0],[0,0.5]],[[0,0],[1,0]],[[0,1],[1,1]],[[0,0.5],[1,0.5]],[[1,0.5],[1,1]]],
't':[[[0,0],[1,0]],[[0.5,0],[0.5,1]]],
'u':[[[0,1],[0,0]],[[0,1],[1,1]],[[1,0],[1,1]]],
'v':[[[0,0],[0.5,1]],[[0.5,1],[1,0]]],
'w':[[[0,0],[0.25,1]],[[0.25,1],[0.5,0.5]],[[0.5,0.5],[0.75,1]],[[0.75,1],[1,0]]],
'x':[[[0,0],[1,1]],[[0,1],[1,0]]],
'y':[[[0,0],[0.5,0.5]],[[0.5,0.5],[1,0]],[[0.5,0.5],[0.5,1]]],
'z':[[[0,1],[1,0]],[[0,0],[1,0]],[[0,1],[1,1]]],
(" ".asSymbol):[[[0,1],[1,1]],[[0,0.8],[0,1]],[[1,0.8],[1,1]]]
);

w = Window("welcome", Rect(40, 500, h, v));
w.view.background = Color.blue(0.5);
w.onClose = { run = false };
w.front;

pen = Pen;
drawletter={arg which, startx, starty, xscale=100, yscale,prop=1.0;
var data;

yscale= yscale ? xscale;

data= linetext[which];

prop=(round((data.size)*prop).asInteger).max(1);

prop.do({arg i;
var val=data[i];

pen.beginPath;
pen.line(Point(startx+(xscale*val[0][0]),starty+(yscale*val[0][1])),Point(startx+(xscale*val[1][0]),starty+(yscale*val[1][1])));
pen.stroke;

});

};


yellowness=rrand(0.7,0.9);

penwidth=rrand(2,3);

w.drawFunc = {

	pen.use {var xoscil, xsizoscil,yoscil, todraw, usedtime;

		pen.width= penwidth;
		pen.color=Color.yellow(yellowness);
		
		usedtime=time.min(1.0);
	
		todraw=(round((name.size)*usedtime).asInteger).max(1);
		
		todraw.do({arg j;
		
		xoscil= sin(2*pi*time+(j*pi*0.13))*140/(1+(10*time));
		
		yoscil= sin(2*pi*time+(j*pi*0.03))*200/(1+(200*time));
		
		xsizoscil= time*5+5;
		
		drawletter.value(name[j],50+(25*j)+(xoscil),10+yoscil,xsizoscil,xsizoscil,usedtime);
		
		});
		
	};
		
		
};

{ while { time<2.0 } { 

w.refresh; 		
time=(time+0.025); //%2.0;
0.05.wait; } }.fork(AppClock)

)
