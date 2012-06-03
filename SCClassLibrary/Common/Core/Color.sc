Color {
	var <>red, <>green, <>blue, <>alpha;

	*new { arg red=0.0, green=0.0, blue=0.0, alpha=1.0;
		^super.newCopyArgs(red, green, blue, alpha);
	}
	*new255 { arg red=0, green=0, blue=0, alpha=255;
		^super.newCopyArgs(red/255, green/255, blue/255, alpha/255);
	}
	*fromArray { arg array;
		^this.new(*array)
	}
	*black { ^Color.new(0.0, 0.0, 0.0) }
	*white { ^Color.new(1.0, 1.0, 1.0) }
	*red { arg val = 1.0, alpha = 1.0; ^Color.new(min(1,val), max(val-1,0), max(val-1,0), alpha) }
	*green { arg val = 1.0, alpha = 1.0; ^Color.new(max(val-1,0), min(1,val), max(val-1,0), alpha) }
	*blue { arg val = 1.0, alpha = 1.0; ^Color.new(max(val-1,0), max(val-1,0), min(1,val), alpha) }
	*cyan { arg val = 1.0, alpha = 1.0; ^Color.new(max(val-1,0), min(1,val), min(1,val), alpha) }
	*magenta { arg val = 1.0, alpha = 1.0; ^Color.new(min(1,val), max(val-1,0), min(1,val), alpha) }
	*yellow { arg val = 1.0, alpha = 1.0; ^Color.new(min(1,val), min(1,val), max(val-1,0), alpha) }
	*clear { ^Color.new(0.0, 0.0, 0.0, 0.0) }
	*grey { arg grey = 0.5, alpha = 1.0;
		^Color.new(grey, grey, grey, alpha);
	}
	*gray { arg gray = 0.5, alpha = 1.0;
		// synonym
		^Color.grey(gray, alpha);
	}

	*rand { arg lo=0.3,hi=0.9; ^Color.new(rrand(lo,hi),rrand(lo,hi),rrand(lo,hi)) }

	== { arg that;
		^this.compareObject(that, #[\red, \green, \blue, \alpha])
	}

	hash {
		^this.instVarHash(#[\red, \green, \blue, \alpha])
	}

	scaleByAlpha {
		^Color.new(red * alpha, green * alpha, blue * alpha, 1.0)
	}
	blend { arg that, blend = 0.5;
		^Color.fromArray(blend(this.asArray, that.asArray, blend));
	}
	vary { arg val=0.1, lo=0.3, hi=0.9, alphaVal=0;
		^Color.new(
			(red + val.rand2).clip(lo,hi),
			(green + val.rand2).clip(lo,hi),
			(blue + val.rand2).clip(lo,hi),
			(alpha + alphaVal.rand2).clip(0,1)
		)
	}
	round { arg val=0.01;
		^Color.fromArray([red, green, blue].round(val) ++ alpha)
	}

	complementary {
		^Color.new(1.0 - red, 1.0 - green, 1.0 - blue, alpha)
	}
	multiply { arg aColor, opacity=1.0;
		var vals = aColor.asArray;
		^Color.fromArray(blend(vals, vals * this.asArray, opacity) ++ alpha)
	}
	divide { arg aColor, opacity=1.0;
		var vals = aColor.asArray, d=0.0001 ! 3;
		^Color.fromArray(blend(vals, ((this.asArray + d) / vals).min(1.0), opacity) ++ alpha)
	}
	subtract { arg aColor, opacity=1.0;
		var vals = aColor.asArray;
		^Color.fromArray(blend(vals, (this.asArray - vals).max(0.0), opacity) ++ alpha)
	}
	add { arg aColor, opacity=1.0;
		var vals = aColor.asArray;
		^Color.fromArray(blend(vals, (vals + this.asArray).min(1.0), opacity) ++ alpha)
	}
	symmetricDifference { arg aColor, opacity=1.0;
		var vals = aColor.asArray;
		^Color.fromArray(blend(vals, abs(vals - this.asArray), opacity) ++ alpha)
	}
	screen { arg aColor, opacity=1.0;
		var vals = aColor.asArray;
		^Color.fromArray(blend(vals, (1-vals) * (1-this.asArray), opacity) ++ alpha)
	}
	lighten  { arg aColor, opacity=1.0;
		var vals = aColor.asArray;
		^Color.fromArray(blend(vals, max(vals, this.asArray), opacity) ++ alpha)
	}
	darken { arg aColor, opacity=1.0;
		var vals = aColor.asArray;
		^Color.fromArray(blend(vals, min(vals, this.asArray), opacity) ++ alpha)
	}
	hueBlend { arg aColor, blend=0.0;
		var f, b;
		f = this.asHSV;
		b = aColor.asHSV;
		^Color.hsv(blend(f[0], b[0], blend), b[1], b[2], alpha)
	}
	saturationBlend { arg aColor, blend=0.0;
		var f, b;
		f = this.asHSV;
		b = aColor.asHSV;
		^Color.hsv(b[0], blend(f[1], b[1], blend), b[2], alpha)
	}
	valueBlend { arg aColor, blend=0.0;
		var f, b;
		f = this.asHSV;
		b = aColor.asHSV;
		^Color.hsv(b[0], b[1], blend(f[2], b[2], blend), alpha)
	}

	*hsv { arg hue, sat, val, alpha=1;
			var r, g, b, segment, fraction, t1, t2, t3;
			hue = hue.linlin(0, 1, 0, 360);
			if( sat == 0 )
				{ r = g = b = val }
				{
						segment = floor( hue/60 )%6;
						fraction = ( hue/60 - segment );
						t1 = val * (1 - sat);
						t2 = val * (1 - (sat * fraction));
						t3 = val * (1 - (sat * (1 - fraction)));
						if( segment == 0, { r=val; g=t3; b=t1 });
						if( segment == 1, { r=t2; g = val; b=t1 });
						if( segment == 2, { r=t1; g=val; b=t3 });
						if( segment == 3, { r=t1; g=t2; b=val });
						if( segment == 4, { r=t3; g=t1; b=val });
						if( segment == 5, { r=val; g=t1; b=t2 });
				};
			//[r, g, b].postln;
			^this.new(r, g, b, alpha);
	}

	asHSV {
		var max, min, delta, hue, sat, val;
		max = [red,green,blue].maxItem;
		min = [red,green,blue].minItem;
		delta = max - min;
		if (red == max, {hue = (green - blue) / delta});
		if (green == max, {hue = (blue - red) / delta + 2});
		if (blue == max, {hue = (red - green) / delta + 4});
		hue = hue/6;
		if (hue < 0, {hue = hue + 1});
	 	sat = delta / max;
		val = max;
		^[hue, sat, val, alpha]
	}

	asArray { ^[red, green, blue, alpha] }

	printOn { arg stream;
		var title;
		stream << this.class.name;
		this.storeParamsOn(stream);
	}
	storeArgs { ^[red,green,blue,alpha] }

	// FIXME The following GUI redirections are rather ugly.
	// Shall we make GUI.color instead, and then CocoaColor, SwingColor, QColor etc. ?

	setStroke {
		if( GUI.id === \cocoa ) { ^this.cocoaPrSetStroke } { Pen.strokeColor = this; }
	}

	setFill {
		if( GUI.id === \cocoa ) { ^this.cocoaPrSetFill } { Pen.fillColor = this; }
	}

	set {
		this.setStroke.setFill;
	}
	
	hexString { // ignores alpha
		var hexRed, hexGreen, hexBlue;
		#hexRed, hexGreen, hexBlue = [red, green, blue] * 255;
		^$# ++ hexRed.round.asInteger.asStringToBase(16, 2)
			++ hexGreen.round.asInteger.asStringToBase(16, 2)
			++ hexBlue.round.asInteger.asStringToBase(16, 2)

	}
	
	*fromHexString {|string|
		var red, green, blue;
		if(string[0] == $#, {string = string.copyToEnd(1)});
		if(string.size == 3, {string = string[0] ++ string[0] ++ string[1] ++ string[1] ++ string[2] ++ string[2]});
		red = ("0x" ++ string.copyRange(0, 1)).interpret;
		green = ("0x" ++ string.copyRange(2, 3)).interpret;
		blue = ("0x" ++ string.copyRange(4, 5)).interpret;
		^this.new255(red, green, blue, 255);
	}	
}

/*	X-windows colors :

(
    alice blue:	Color.new255(240, 248, 255),
    AliceBlue:	Color.new255(240, 248, 255),
    antique white:	Color.new255(250, 235, 215),
    AntiqueWhite:	Color.new255(250, 235, 215),
    AntiqueWhite1:	Color.new255(255, 239, 219),
    AntiqueWhite2:	Color.new255(238, 223, 204),
    AntiqueWhite3:	Color.new255(205, 192, 176),
    AntiqueWhite4:	Color.new255(139, 131, 120),
    aquamarine:	Color.new255(127, 255, 212),
    aquamarine1:	Color.new255(127, 255, 212),
    aquamarine2:	Color.new255(118, 238, 198),
    aquamarine3:	Color.new255(102, 205, 170),
    aquamarine4:	Color.new255(69, 139, 116),
    azure:	Color.new255(240, 255, 255),
    azure1:	Color.new255(240, 255, 255),
    azure2:	Color.new255(224, 238, 238),
    azure3:	Color.new255(193, 205, 205),
    azure4:	Color.new255(131, 139, 139),
    beige:	Color.new255(245, 245, 220),
    bisque:	Color.new255(255, 228, 196),
    bisque1:	Color.new255(255, 228, 196),
    bisque2:	Color.new255(238, 213, 183),
    bisque3:	Color.new255(205, 183, 158),
    bisque4:	Color.new255(139, 125, 107),
    black:	Color.new255(0, 0, 0),
    blanched almond:	Color.new255(255, 235, 205),
    BlanchedAlmond:	Color.new255(255, 235, 205),
    blue:	Color.new255(0, 0, 255),
    blue violet:	Color.new255(138, 43, 226),
    blue1:	Color.new255(0, 0, 255),
    blue2:	Color.new255(0, 0, 238),
    blue3:	Color.new255(0, 0, 205),
    blue4:	Color.new255(0, 0, 139),
    BlueViolet:	Color.new255(138, 43, 226),
    brown:	Color.new255(165, 42, 42),
    brown1:	Color.new255(255, 64, 64),
    brown2:	Color.new255(238, 59, 59),
    brown3:	Color.new255(205, 51, 51),
    brown4:	Color.new255(139, 35, 35),
    burlywood:	Color.new255(222, 184, 135),
    burlywood1:	Color.new255(255, 211, 155),
    burlywood2:	Color.new255(238, 197, 145),
    burlywood3:	Color.new255(205, 170, 125),
    burlywood4:	Color.new255(139, 115, 85),
    cadet blue:	Color.new255(95, 158, 160),
    CadetBlue:	Color.new255(95, 158, 160),
    CadetBlue1:	Color.new255(152, 245, 255),
    CadetBlue2:	Color.new255(142, 229, 238),
    CadetBlue3:	Color.new255(122, 197, 205),
    CadetBlue4:	Color.new255(83, 134, 139),
    chartreuse:	Color.new255(127, 255, 0),
    chartreuse1:	Color.new255(127, 255, 0),
    chartreuse2:	Color.new255(118, 238, 0),
    chartreuse3:	Color.new255(102, 205, 0),
    chartreuse4:	Color.new255(69, 139, 0),
    chocolate:	Color.new255(210, 105, 30),
    chocolate1:	Color.new255(255, 127, 36),
    chocolate2:	Color.new255(238, 118, 33),
    chocolate3:	Color.new255(205, 102, 29),
    chocolate4:	Color.new255(139, 69, 19),
    coral:	Color.new255(255, 127, 80),
    coral1:	Color.new255(255, 114, 86),
    coral2:	Color.new255(238, 106, 80),
    coral3:	Color.new255(205, 91, 69),
    coral4:	Color.new255(139, 62, 47),
    cornflower blue:	Color.new255(100, 149, 237),
    CornflowerBlue:	Color.new255(100, 149, 237),
    cornsilk:	Color.new255(255, 248, 220),
    cornsilk1:	Color.new255(255, 248, 220),
    cornsilk2:	Color.new255(238, 232, 205),
    cornsilk3:	Color.new255(205, 200, 177),
    cornsilk4:	Color.new255(139, 136, 120),
    cyan:	Color.new255(0, 255, 255),
    cyan1:	Color.new255(0, 255, 255),
    cyan2:	Color.new255(0, 238, 238),
    cyan3:	Color.new255(0, 205, 205),
    cyan4:	Color.new255(0, 139, 139),
    dark goldenrod:	Color.new255(184, 134, 11),
    dark green:	Color.new255(0, 100, 0),
    dark khaki:	Color.new255(189, 183, 107),
    dark olive green:	Color.new255(85, 107, 47),
    dark orange:	Color.new255(255, 140, 0),
    dark orchid:	Color.new255(153, 50, 204),
    dark salmon:	Color.new255(233, 150, 122),
    dark sea green:	Color.new255(143, 188, 143),
    dark slate blue:	Color.new255(72, 61, 139),
    dark slate gray:	Color.new255(47, 79, 79),
    dark slate grey:	Color.new255(47, 79, 79),
    dark turquoise:	Color.new255(0, 206, 209),
    dark violet:	Color.new255(148, 0, 211),
    DarkGoldenrod:	Color.new255(184, 134, 11),
    DarkGoldenrod1:	Color.new255(255, 185, 15),
    DarkGoldenrod2:	Color.new255(238, 173, 14),
    DarkGoldenrod3:	Color.new255(205, 149, 12),
    DarkGoldenrod4:	Color.new255(139, 101, 8),
    DarkGreen:	Color.new255(0, 100, 0),
    DarkKhaki:	Color.new255(189, 183, 107),
    DarkOliveGreen:	Color.new255(85, 107, 47),
    DarkOliveGreen1:	Color.new255(202, 255, 112),
    DarkOliveGreen2:	Color.new255(188, 238, 104),
    DarkOliveGreen3:	Color.new255(162, 205, 90),
    DarkOliveGreen4:	Color.new255(110, 139, 61),
    DarkOrange:	Color.new255(255, 140, 0),
    DarkOrange1:	Color.new255(255, 127, 0),
    DarkOrange2:	Color.new255(238, 118, 0),
    DarkOrange3:	Color.new255(205, 102, 0),
    DarkOrange4:	Color.new255(139, 69, 0),
    DarkOrchid:	Color.new255(153, 50, 204),
    DarkOrchid1:	Color.new255(191, 62, 255),
    DarkOrchid2:	Color.new255(178, 58, 238),
    DarkOrchid3:	Color.new255(154, 50, 205),
    DarkOrchid4:	Color.new255(104, 34, 139),
    DarkSalmon:	Color.new255(233, 150, 122),
    DarkSeaGreen:	Color.new255(143, 188, 143),
    DarkSeaGreen1:	Color.new255(193, 255, 193),
    DarkSeaGreen2:	Color.new255(180, 238, 180),
    DarkSeaGreen3:	Color.new255(155, 205, 155),
    DarkSeaGreen4:	Color.new255(105, 139, 105),
    DarkSlateBlue:	Color.new255(72, 61, 139),
    DarkSlateGray:	Color.new255(47, 79, 79),
    DarkSlateGray1:	Color.new255(151, 255, 255),
    DarkSlateGray2:	Color.new255(141, 238, 238),
    DarkSlateGray3:	Color.new255(121, 205, 205),
    DarkSlateGray4:	Color.new255(82, 139, 139),
    DarkSlateGrey:	Color.new255(47, 79, 79),
    DarkTurquoise:	Color.new255(0, 206, 209),
    DarkViolet:	Color.new255(148, 0, 211),
    deep pink:	Color.new255(255, 20, 147),
    deep sky blue:	Color.new255(0, 191, 255),
    DeepPink:	Color.new255(255, 20, 147),
    DeepPink1:	Color.new255(255, 20, 147),
    DeepPink2:	Color.new255(238, 18, 137),
    DeepPink3:	Color.new255(205, 16, 118),
    DeepPink4:	Color.new255(139, 10, 80),
    DeepSkyBlue:	Color.new255(0, 191, 255),
    DeepSkyBlue1:	Color.new255(0, 191, 255),
    DeepSkyBlue2:	Color.new255(0, 178, 238),
    DeepSkyBlue3:	Color.new255(0, 154, 205),
    DeepSkyBlue4:	Color.new255(0, 104, 139),
    dim gray:	Color.new255(105, 105, 105),
    dim grey:	Color.new255(105, 105, 105),
    DimGray:	Color.new255(105, 105, 105),
    DimGrey:	Color.new255(105, 105, 105),
    dodger blue:	Color.new255(30, 144, 255),
    DodgerBlue:	Color.new255(30, 144, 255),
    DodgerBlue1:	Color.new255(30, 144, 255),
    DodgerBlue2:	Color.new255(28, 134, 238),
    DodgerBlue3:	Color.new255(24, 116, 205),
    DodgerBlue4:	Color.new255(16, 78, 139),
    firebrick:	Color.new255(178, 34, 34),
    firebrick1:	Color.new255(255, 48, 48),
    firebrick2:	Color.new255(238, 44, 44),
    firebrick3:	Color.new255(205, 38, 38),
    firebrick4:	Color.new255(139, 26, 26),
    floral white:	Color.new255(255, 250, 240),
    FloralWhite:	Color.new255(255, 250, 240),
    forest green:	Color.new255(34, 139, 34),
    ForestGreen:	Color.new255(34, 139, 34),
    gainsboro:	Color.new255(220, 220, 220),
    ghost white:	Color.new255(248, 248, 255),
    GhostWhite:	Color.new255(248, 248, 255),
    gold:	Color.new255(255, 215, 0),
    gold1:	Color.new255(255, 215, 0),
    gold2:	Color.new255(238, 201, 0),
    gold3:	Color.new255(205, 173, 0),
    gold4:	Color.new255(139, 117, 0),
    goldenrod:	Color.new255(218, 165, 32),
    goldenrod1:	Color.new255(255, 193, 37),
    goldenrod2:	Color.new255(238, 180, 34),
    goldenrod3:	Color.new255(205, 155, 29),
    goldenrod4:	Color.new255(139, 105, 20),
    gray:	Color.new255(190, 190, 190),
    gray0:	Color.new255(0, 0, 0),
    gray1:	Color.new255(3, 3, 3),
    gray10:	Color.new255(26, 26, 26),
    gray100:	Color.new255(255, 255, 255),
    gray11:	Color.new255(28, 28, 28),
    gray12:	Color.new255(31, 31, 31),
    gray13:	Color.new255(33, 33, 33),
    gray14:	Color.new255(36, 36, 36),
    gray15:	Color.new255(38, 38, 38),
    gray16:	Color.new255(41, 41, 41),
    gray17:	Color.new255(43, 43, 43),
    gray18:	Color.new255(46, 46, 46),
    gray19:	Color.new255(48, 48, 48),
    gray2:	Color.new255(5, 5, 5),
    gray20:	Color.new255(51, 51, 51),
    gray21:	Color.new255(54, 54, 54),
    gray22:	Color.new255(56, 56, 56),
    gray23:	Color.new255(59, 59, 59),
    gray24:	Color.new255(61, 61, 61),
    gray25:	Color.new255(64, 64, 64),
    gray26:	Color.new255(66, 66, 66),
    gray27:	Color.new255(69, 69, 69),
    gray28:	Color.new255(71, 71, 71),
    gray29:	Color.new255(74, 74, 74),
    gray3:	Color.new255(8, 8, 8),
    gray30:	Color.new255(77, 77, 77),
    gray31:	Color.new255(79, 79, 79),
    gray32:	Color.new255(82, 82, 82),
    gray33:	Color.new255(84, 84, 84),
    gray34:	Color.new255(87, 87, 87),
    gray35:	Color.new255(89, 89, 89),
    gray36:	Color.new255(92, 92, 92),
    gray37:	Color.new255(94, 94, 94),
    gray38:	Color.new255(97, 97, 97),
    gray39:	Color.new255(99, 99, 99),
    gray4:	Color.new255(10, 10, 10),
    gray40:	Color.new255(102, 102, 102),
    gray41:	Color.new255(105, 105, 105),
    gray42:	Color.new255(107, 107, 107),
    gray43:	Color.new255(110, 110, 110),
    gray44:	Color.new255(112, 112, 112),
    gray45:	Color.new255(115, 115, 115),
    gray46:	Color.new255(117, 117, 117),
    gray47:	Color.new255(120, 120, 120),
    gray48:	Color.new255(122, 122, 122),
    gray49:	Color.new255(125, 125, 125),
    gray5:	Color.new255(13, 13, 13),
    gray50:	Color.new255(127, 127, 127),
    gray51:	Color.new255(130, 130, 130),
    gray52:	Color.new255(133, 133, 133),
    gray53:	Color.new255(135, 135, 135),
    gray54:	Color.new255(138, 138, 138),
    gray55:	Color.new255(140, 140, 140),
    gray56:	Color.new255(143, 143, 143),
    gray57:	Color.new255(145, 145, 145),
    gray58:	Color.new255(148, 148, 148),
    gray59:	Color.new255(150, 150, 150),
    gray6:	Color.new255(15, 15, 15),
    gray60:	Color.new255(153, 153, 153),
    gray61:	Color.new255(156, 156, 156),
    gray62:	Color.new255(158, 158, 158),
    gray63:	Color.new255(161, 161, 161),
    gray64:	Color.new255(163, 163, 163),
    gray65:	Color.new255(166, 166, 166),
    gray66:	Color.new255(168, 168, 168),
    gray67:	Color.new255(171, 171, 171),
    gray68:	Color.new255(173, 173, 173),
    gray69:	Color.new255(176, 176, 176),
    gray7:	Color.new255(18, 18, 18),
    gray70:	Color.new255(179, 179, 179),
    gray71:	Color.new255(181, 181, 181),
    gray72:	Color.new255(184, 184, 184),
    gray73:	Color.new255(186, 186, 186),
    gray74:	Color.new255(189, 189, 189),
    gray75:	Color.new255(191, 191, 191),
    gray76:	Color.new255(194, 194, 194),
    gray77:	Color.new255(196, 196, 196),
    gray78:	Color.new255(199, 199, 199),
    gray79:	Color.new255(201, 201, 201),
    gray8:	Color.new255(20, 20, 20),
    gray80:	Color.new255(204, 204, 204),
    gray81:	Color.new255(207, 207, 207),
    gray82:	Color.new255(209, 209, 209),
    gray83:	Color.new255(212, 212, 212),
    gray84:	Color.new255(214, 214, 214),
    gray85:	Color.new255(217, 217, 217),
    gray86:	Color.new255(219, 219, 219),
    gray87:	Color.new255(222, 222, 222),
    gray88:	Color.new255(224, 224, 224),
    gray89:	Color.new255(227, 227, 227),
    gray9:	Color.new255(23, 23, 23),
    gray90:	Color.new255(229, 229, 229),
    gray91:	Color.new255(232, 232, 232),
    gray92:	Color.new255(235, 235, 235),
    gray93:	Color.new255(237, 237, 237),
    gray94:	Color.new255(240, 240, 240),
    gray95:	Color.new255(242, 242, 242),
    gray96:	Color.new255(245, 245, 245),
    gray97:	Color.new255(247, 247, 247),
    gray98:	Color.new255(250, 250, 250),
    gray99:	Color.new255(252, 252, 252),
    green:	Color.new255(0, 255, 0),
    green yellow:	Color.new255(173, 255, 47),
    green1:	Color.new255(0, 255, 0),
    green2:	Color.new255(0, 238, 0),
    green3:	Color.new255(0, 205, 0),
    green4:	Color.new255(0, 139, 0),
    GreenYellow:	Color.new255(173, 255, 47),
    grey:	Color.new255(190, 190, 190),
    grey0:	Color.new255(0, 0, 0),
    grey1:	Color.new255(3, 3, 3),
    grey10:	Color.new255(26, 26, 26),
    grey100:	Color.new255(255, 255, 255),
    grey11:	Color.new255(28, 28, 28),
    grey12:	Color.new255(31, 31, 31),
    grey13:	Color.new255(33, 33, 33),
    grey14:	Color.new255(36, 36, 36),
    grey15:	Color.new255(38, 38, 38),
    grey16:	Color.new255(41, 41, 41),
    grey17:	Color.new255(43, 43, 43),
    grey18:	Color.new255(46, 46, 46),
    grey19:	Color.new255(48, 48, 48),
    grey2:	Color.new255(5, 5, 5),
    grey20:	Color.new255(51, 51, 51),
    grey21:	Color.new255(54, 54, 54),
    grey22:	Color.new255(56, 56, 56),
    grey23:	Color.new255(59, 59, 59),
    grey24:	Color.new255(61, 61, 61),
    grey25:	Color.new255(64, 64, 64),
    grey26:	Color.new255(66, 66, 66),
    grey27:	Color.new255(69, 69, 69),
    grey28:	Color.new255(71, 71, 71),
    grey29:	Color.new255(74, 74, 74),
    grey3:	Color.new255(8, 8, 8),
    grey30:	Color.new255(77, 77, 77),
    grey31:	Color.new255(79, 79, 79),
    grey32:	Color.new255(82, 82, 82),
    grey33:	Color.new255(84, 84, 84),
    grey34:	Color.new255(87, 87, 87),
    grey35:	Color.new255(89, 89, 89),
    grey36:	Color.new255(92, 92, 92),
    grey37:	Color.new255(94, 94, 94),
    grey38:	Color.new255(97, 97, 97),
    grey39:	Color.new255(99, 99, 99),
    grey4:	Color.new255(10, 10, 10),
    grey40:	Color.new255(102, 102, 102),
    grey41:	Color.new255(105, 105, 105),
    grey42:	Color.new255(107, 107, 107),
    grey43:	Color.new255(110, 110, 110),
    grey44:	Color.new255(112, 112, 112),
    grey45:	Color.new255(115, 115, 115),
    grey46:	Color.new255(117, 117, 117),
    grey47:	Color.new255(120, 120, 120),
    grey48:	Color.new255(122, 122, 122),
    grey49:	Color.new255(125, 125, 125),
    grey5:	Color.new255(13, 13, 13),
    grey50:	Color.new255(127, 127, 127),
    grey51:	Color.new255(130, 130, 130),
    grey52:	Color.new255(133, 133, 133),
    grey53:	Color.new255(135, 135, 135),
    grey54:	Color.new255(138, 138, 138),
    grey55:	Color.new255(140, 140, 140),
    grey56:	Color.new255(143, 143, 143),
    grey57:	Color.new255(145, 145, 145),
    grey58:	Color.new255(148, 148, 148),
    grey59:	Color.new255(150, 150, 150),
    grey6:	Color.new255(15, 15, 15),
    grey60:	Color.new255(153, 153, 153),
    grey61:	Color.new255(156, 156, 156),
    grey62:	Color.new255(158, 158, 158),
    grey63:	Color.new255(161, 161, 161),
    grey64:	Color.new255(163, 163, 163),
    grey65:	Color.new255(166, 166, 166),
    grey66:	Color.new255(168, 168, 168),
    grey67:	Color.new255(171, 171, 171),
    grey68:	Color.new255(173, 173, 173),
    grey69:	Color.new255(176, 176, 176),
    grey7:	Color.new255(18, 18, 18),
    grey70:	Color.new255(179, 179, 179),
    grey71:	Color.new255(181, 181, 181),
    grey72:	Color.new255(184, 184, 184),
    grey73:	Color.new255(186, 186, 186),
    grey74:	Color.new255(189, 189, 189),
    grey75:	Color.new255(191, 191, 191),
    grey76:	Color.new255(194, 194, 194),
    grey77:	Color.new255(196, 196, 196),
    grey78:	Color.new255(199, 199, 199),
    grey79:	Color.new255(201, 201, 201),
    grey8:	Color.new255(20, 20, 20),
    grey80:	Color.new255(204, 204, 204),
    grey81:	Color.new255(207, 207, 207),
    grey82:	Color.new255(209, 209, 209),
    grey83:	Color.new255(212, 212, 212),
    grey84:	Color.new255(214, 214, 214),
    grey85:	Color.new255(217, 217, 217),
    grey86:	Color.new255(219, 219, 219),
    grey87:	Color.new255(222, 222, 222),
    grey88:	Color.new255(224, 224, 224),
    grey89:	Color.new255(227, 227, 227),
    grey9:	Color.new255(23, 23, 23),
    grey90:	Color.new255(229, 229, 229),
    grey91:	Color.new255(232, 232, 232),
    grey92:	Color.new255(235, 235, 235),
    grey93:	Color.new255(237, 237, 237),
    grey94:	Color.new255(240, 240, 240),
    grey95:	Color.new255(242, 242, 242),
    grey96:	Color.new255(245, 245, 245),
    grey97:	Color.new255(247, 247, 247),
    grey98:	Color.new255(250, 250, 250),
    grey99:	Color.new255(252, 252, 252),
    honeydew:	Color.new255(240, 255, 240),
    honeydew1:	Color.new255(240, 255, 240),
    honeydew2:	Color.new255(224, 238, 224),
    honeydew3:	Color.new255(193, 205, 193),
    honeydew4:	Color.new255(131, 139, 131),
    hot pink:	Color.new255(255, 105, 180),
    HotPink:	Color.new255(255, 105, 180),
    HotPink1:	Color.new255(255, 110, 180),
    HotPink2:	Color.new255(238, 106, 167),
    HotPink3:	Color.new255(205, 96, 144),
    HotPink4:	Color.new255(139, 58, 98),
    indian red:	Color.new255(205, 92, 92),
    IndianRed:	Color.new255(205, 92, 92),
    IndianRed1:	Color.new255(255, 106, 106),
    IndianRed2:	Color.new255(238, 99, 99),
    IndianRed3:	Color.new255(205, 85, 85),
    IndianRed4:	Color.new255(139, 58, 58),
    ivory:	Color.new255(255, 255, 240),
    ivory1:	Color.new255(255, 255, 240),
    ivory2:	Color.new255(238, 238, 224),
    ivory3:	Color.new255(205, 205, 193),
    ivory4:	Color.new255(139, 139, 131),
    khaki:	Color.new255(240, 230, 140),
    khaki1:	Color.new255(255, 246, 143),
    khaki2:	Color.new255(238, 230, 133),
    khaki3:	Color.new255(205, 198, 115),
    khaki4:	Color.new255(139, 134, 78),
    lavender:	Color.new255(230, 230, 250),
    lavender blush:	Color.new255(255, 240, 245),
    LavenderBlush:	Color.new255(255, 240, 245),
    LavenderBlush1:	Color.new255(255, 240, 245),
    LavenderBlush2:	Color.new255(238, 224, 229),
    LavenderBlush3:	Color.new255(205, 193, 197),
    LavenderBlush4:	Color.new255(139, 131, 134),
    lawn green:	Color.new255(124, 252, 0),
    LawnGreen:	Color.new255(124, 252, 0),
    lemon chiffon:	Color.new255(255, 250, 205),
    LemonChiffon:	Color.new255(255, 250, 205),
    LemonChiffon1:	Color.new255(255, 250, 205),
    LemonChiffon2:	Color.new255(238, 233, 191),
    LemonChiffon3:	Color.new255(205, 201, 165),
    LemonChiffon4:	Color.new255(139, 137, 112),
    light blue:	Color.new255(173, 216, 230),
    light coral:	Color.new255(240, 128, 128),
    light cyan:	Color.new255(224, 255, 255),
    light goldenrod:	Color.new255(238, 221, 130),
    light goldenrod yellow:	Color.new255(250, 250, 210),
    light gray:	Color.new255(211, 211, 211),
    light grey:	Color.new255(211, 211, 211),
    light pink:	Color.new255(255, 182, 193),
    light salmon:	Color.new255(255, 160, 122),
    light sea green:	Color.new255(32, 178, 170),
    light sky blue:	Color.new255(135, 206, 250),
    light slate blue:	Color.new255(132, 112, 255),
    light slate gray:	Color.new255(119, 136, 153),
    light slate grey:	Color.new255(119, 136, 153),
    light steel blue:	Color.new255(176, 196, 222),
    light yellow:	Color.new255(255, 255, 224),
    LightBlue:	Color.new255(173, 216, 230),
    LightBlue1:	Color.new255(191, 239, 255),
    LightBlue2:	Color.new255(178, 223, 238),
    LightBlue3:	Color.new255(154, 192, 205),
    LightBlue4:	Color.new255(104, 131, 139),
    LightCoral:	Color.new255(240, 128, 128),
    LightCyan:	Color.new255(224, 255, 255),
    LightCyan1:	Color.new255(224, 255, 255),
    LightCyan2:	Color.new255(209, 238, 238),
    LightCyan3:	Color.new255(180, 205, 205),
    LightCyan4:	Color.new255(122, 139, 139),
    LightGoldenrod:	Color.new255(238, 221, 130),
    LightGoldenrod1:	Color.new255(255, 236, 139),
    LightGoldenrod2:	Color.new255(238, 220, 130),
    LightGoldenrod3:	Color.new255(205, 190, 112),
    LightGoldenrod4:	Color.new255(139, 129, 76),
    LightGoldenrodYellow:	Color.new255(250, 250, 210),
    LightGray:	Color.new255(211, 211, 211),
    LightGrey:	Color.new255(211, 211, 211),
    LightPink:	Color.new255(255, 182, 193),
    LightPink1:	Color.new255(255, 174, 185),
    LightPink2:	Color.new255(238, 162, 173),
    LightPink3:	Color.new255(205, 140, 149),
    LightPink4:	Color.new255(139, 95, 101),
    LightSalmon:	Color.new255(255, 160, 122),
    LightSalmon1:	Color.new255(255, 160, 122),
    LightSalmon2:	Color.new255(238, 149, 114),
    LightSalmon3:	Color.new255(205, 129, 98),
    LightSalmon4:	Color.new255(139, 87, 66),
    LightSeaGreen:	Color.new255(32, 178, 170),
    LightSkyBlue:	Color.new255(135, 206, 250),
    LightSkyBlue1:	Color.new255(176, 226, 255),
    LightSkyBlue2:	Color.new255(164, 211, 238),
    LightSkyBlue3:	Color.new255(141, 182, 205),
    LightSkyBlue4:	Color.new255(96, 123, 139),
    LightSlateBlue:	Color.new255(132, 112, 255),
    LightSlateGray:	Color.new255(119, 136, 153),
    LightSlateGrey:	Color.new255(119, 136, 153),
    LightSteelBlue:	Color.new255(176, 196, 222),
    LightSteelBlue1:	Color.new255(202, 225, 255),
    LightSteelBlue2:	Color.new255(188, 210, 238),
    LightSteelBlue3:	Color.new255(162, 181, 205),
    LightSteelBlue4:	Color.new255(110, 123, 139),
    LightYellow:	Color.new255(255, 255, 224),
    LightYellow1:	Color.new255(255, 255, 224),
    LightYellow2:	Color.new255(238, 238, 209),
    LightYellow3:	Color.new255(205, 205, 180),
    LightYellow4:	Color.new255(139, 139, 122),
    lime green:	Color.new255(50, 205, 50),
    LimeGreen:	Color.new255(50, 205, 50),
    linen:	Color.new255(250, 240, 230),
    magenta:	Color.new255(255, 0, 255),
    magenta1:	Color.new255(255, 0, 255),
    magenta2:	Color.new255(238, 0, 238),
    magenta3:	Color.new255(205, 0, 205),
    magenta4:	Color.new255(139, 0, 139),
    maroon:	Color.new255(176, 48, 96),
    maroon1:	Color.new255(255, 52, 179),
    maroon2:	Color.new255(238, 48, 167),
    maroon3:	Color.new255(205, 41, 144),
    maroon4:	Color.new255(139, 28, 98),
    medium aquamarine:	Color.new255(102, 205, 170),
    medium blue:	Color.new255(0, 0, 205),
    medium orchid:	Color.new255(186, 85, 211),
    medium purple:	Color.new255(147, 112, 219),
    medium sea green:	Color.new255(60, 179, 113),
    medium slate blue:	Color.new255(123, 104, 238),
    medium spring green:	Color.new255(0, 250, 154),
    medium turquoise:	Color.new255(72, 209, 204),
    medium violet red:	Color.new255(199, 21, 133),
    MediumAquamarine:	Color.new255(102, 205, 170),
    MediumBlue:	Color.new255(0, 0, 205),
    MediumOrchid:	Color.new255(186, 85, 211),
    MediumOrchid1:	Color.new255(224, 102, 255),
    MediumOrchid2:	Color.new255(209, 95, 238),
    MediumOrchid3:	Color.new255(180, 82, 205),
    MediumOrchid4:	Color.new255(122, 55, 139),
    MediumPurple:	Color.new255(147, 112, 219),
    MediumPurple1:	Color.new255(171, 130, 255),
    MediumPurple2:	Color.new255(159, 121, 238),
    MediumPurple3:	Color.new255(137, 104, 205),
    MediumPurple4:	Color.new255(93, 71, 139),
    MediumSeaGreen:	Color.new255(60, 179, 113),
    MediumSlateBlue:	Color.new255(123, 104, 238),
    MediumSpringGreen:	Color.new255(0, 250, 154),
    MediumTurquoise:	Color.new255(72, 209, 204),
    MediumVioletRed:	Color.new255(199, 21, 133),
    midnight blue:	Color.new255(25, 25, 112),
    MidnightBlue:	Color.new255(25, 25, 112),
    mint cream:	Color.new255(245, 255, 250),
    MintCream:	Color.new255(245, 255, 250),
    misty rose:	Color.new255(255, 228, 225),
    MistyRose:	Color.new255(255, 228, 225),
    MistyRose1:	Color.new255(255, 228, 225),
    MistyRose2:	Color.new255(238, 213, 210),
    MistyRose3:	Color.new255(205, 183, 181),
    MistyRose4:	Color.new255(139, 125, 123),
    moccasin:	Color.new255(255, 228, 181),
    navajo white:	Color.new255(255, 222, 173),
    NavajoWhite:	Color.new255(255, 222, 173),
    NavajoWhite1:	Color.new255(255, 222, 173),
    NavajoWhite2:	Color.new255(238, 207, 161),
    NavajoWhite3:	Color.new255(205, 179, 139),
    NavajoWhite4:	Color.new255(139, 121, 94),
    navy:	Color.new255(0, 0, 128),
    navy blue:	Color.new255(0, 0, 128),
    NavyBlue:	Color.new255(0, 0, 128),
    old lace:	Color.new255(253, 245, 230),
    OldLace:	Color.new255(253, 245, 230),
    olive drab:	Color.new255(107, 142, 35),
    OliveDrab:	Color.new255(107, 142, 35),
    OliveDrab1:	Color.new255(192, 255, 62),
    OliveDrab2:	Color.new255(179, 238, 58),
    OliveDrab3:	Color.new255(154, 205, 50),
    OliveDrab4:	Color.new255(105, 139, 34),
    orange:	Color.new255(255, 165, 0),
    orange red:	Color.new255(255, 69, 0),
    orange1:	Color.new255(255, 165, 0),
    orange2:	Color.new255(238, 154, 0),
    orange3:	Color.new255(205, 133, 0),
    orange4:	Color.new255(139, 90, 0),
    OrangeRed:	Color.new255(255, 69, 0),
    OrangeRed1:	Color.new255(255, 69, 0),
    OrangeRed2:	Color.new255(238, 64, 0),
    OrangeRed3:	Color.new255(205, 55, 0),
    OrangeRed4:	Color.new255(139, 37, 0),
    orchid:	Color.new255(218, 112, 214),
    orchid1:	Color.new255(255, 131, 250),
    orchid2:	Color.new255(238, 122, 233),
    orchid3:	Color.new255(205, 105, 201),
    orchid4:	Color.new255(139, 71, 137),
    pale goldenrod:	Color.new255(238, 232, 170),
    pale green:	Color.new255(152, 251, 152),
    pale turquoise:	Color.new255(175, 238, 238),
    pale violet red:	Color.new255(219, 112, 147),
    PaleGoldenrod:	Color.new255(238, 232, 170),
    PaleGreen:	Color.new255(152, 251, 152),
    PaleGreen1:	Color.new255(154, 255, 154),
    PaleGreen2:	Color.new255(144, 238, 144),
    PaleGreen3:	Color.new255(124, 205, 124),
    PaleGreen4:	Color.new255(84, 139, 84),
    PaleTurquoise:	Color.new255(175, 238, 238),
    PaleTurquoise1:	Color.new255(187, 255, 255),
    PaleTurquoise2:	Color.new255(174, 238, 238),
    PaleTurquoise3:	Color.new255(150, 205, 205),
    PaleTurquoise4:	Color.new255(102, 139, 139),
    PaleVioletRed:	Color.new255(219, 112, 147),
    PaleVioletRed1:	Color.new255(255, 130, 171),
    PaleVioletRed2:	Color.new255(238, 121, 159),
    PaleVioletRed3:	Color.new255(205, 104, 137),
    PaleVioletRed4:	Color.new255(139, 71, 93),
    papaya whip:	Color.new255(255, 239, 213),
    PapayaWhip:	Color.new255(255, 239, 213),
    peach puff:	Color.new255(255, 218, 185),
    PeachPuff:	Color.new255(255, 218, 185),
    PeachPuff1:	Color.new255(255, 218, 185),
    PeachPuff2:	Color.new255(238, 203, 173),
    PeachPuff3:	Color.new255(205, 175, 149),
    PeachPuff4:	Color.new255(139, 119, 101),
    peru:	Color.new255(205, 133, 63),
    pink:	Color.new255(255, 192, 203),
    pink1:	Color.new255(255, 181, 197),
    pink2:	Color.new255(238, 169, 184),
    pink3:	Color.new255(205, 145, 158),
    pink4:	Color.new255(139, 99, 108),
    plum:	Color.new255(221, 160, 221),
    plum1:	Color.new255(255, 187, 255),
    plum2:	Color.new255(238, 174, 238),
    plum3:	Color.new255(205, 150, 205),
    plum4:	Color.new255(139, 102, 139),
    powder blue:	Color.new255(176, 224, 230),
    PowderBlue:	Color.new255(176, 224, 230),
    purple:	Color.new255(160, 32, 240),
    purple1:	Color.new255(155, 48, 255),
    purple2:	Color.new255(145, 44, 238),
    purple3:	Color.new255(125, 38, 205),
    purple4:	Color.new255(85, 26, 139),
    red:	Color.new255(255, 0, 0),
    red1:	Color.new255(255, 0, 0),
    red2:	Color.new255(238, 0, 0),
    red3:	Color.new255(205, 0, 0),
    red4:	Color.new255(139, 0, 0),
    rosy brown:	Color.new255(188, 143, 143),
    RosyBrown:	Color.new255(188, 143, 143),
    RosyBrown1:	Color.new255(255, 193, 193),
    RosyBrown2:	Color.new255(238, 180, 180),
    RosyBrown3:	Color.new255(205, 155, 155),
    RosyBrown4:	Color.new255(139, 105, 105),
    royal blue:	Color.new255(65, 105, 225),
    RoyalBlue:	Color.new255(65, 105, 225),
    RoyalBlue1:	Color.new255(72, 118, 255),
    RoyalBlue2:	Color.new255(67, 110, 238),
    RoyalBlue3:	Color.new255(58, 95, 205),
    RoyalBlue4:	Color.new255(39, 64, 139),
    saddle brown:	Color.new255(139, 69, 19),
    SaddleBrown:	Color.new255(139, 69, 19),
    salmon:	Color.new255(250, 128, 114),
    salmon1:	Color.new255(255, 140, 105),
    salmon2:	Color.new255(238, 130, 98),
    salmon3:	Color.new255(205, 112, 84),
    salmon4:	Color.new255(139, 76, 57),
    sandy brown:	Color.new255(244, 164, 96),
    SandyBrown:	Color.new255(244, 164, 96),
    sea green:	Color.new255(46, 139, 87),
    SeaGreen:	Color.new255(46, 139, 87),
    SeaGreen1:	Color.new255(84, 255, 159),
    SeaGreen2:	Color.new255(78, 238, 148),
    SeaGreen3:	Color.new255(67, 205, 128),
    SeaGreen4:	Color.new255(46, 139, 87),
    seashell:	Color.new255(255, 245, 238),
    seashell1:	Color.new255(255, 245, 238),
    seashell2:	Color.new255(238, 229, 222),
    seashell3:	Color.new255(205, 197, 191),
    seashell4:	Color.new255(139, 134, 130),
    sienna:	Color.new255(160, 82, 45),
    sienna1:	Color.new255(255, 130, 71),
    sienna2:	Color.new255(238, 121, 66),
    sienna3:	Color.new255(205, 104, 57),
    sienna4:	Color.new255(139, 71, 38),
    sky blue:	Color.new255(135, 206, 235),
    SkyBlue:	Color.new255(135, 206, 235),
    SkyBlue1:	Color.new255(135, 206, 255),
    SkyBlue2:	Color.new255(126, 192, 238),
    SkyBlue3:	Color.new255(108, 166, 205),
    SkyBlue4:	Color.new255(74, 112, 139),
    slate blue:	Color.new255(106, 90, 205),
    slate gray:	Color.new255(112, 128, 144),
    slate grey:	Color.new255(112, 128, 144),
    SlateBlue:	Color.new255(106, 90, 205),
    SlateBlue1:	Color.new255(131, 111, 255),
    SlateBlue2:	Color.new255(122, 103, 238),
    SlateBlue3:	Color.new255(105, 89, 205),
    SlateBlue4:	Color.new255(71, 60, 139),
    SlateGray:	Color.new255(112, 128, 144),
    SlateGray1:	Color.new255(198, 226, 255),
    SlateGray2:	Color.new255(185, 211, 238),
    SlateGray3:	Color.new255(159, 182, 205),
    SlateGray4:	Color.new255(108, 123, 139),
    SlateGrey:	Color.new255(112, 128, 144),
    snow:	Color.new255(255, 250, 250),
    snow1:	Color.new255(255, 250, 250),
    snow2:	Color.new255(238, 233, 233),
    snow3:	Color.new255(205, 201, 201),
    snow4:	Color.new255(139, 137, 137),
    spring green:	Color.new255(0, 255, 127),
    SpringGreen:	Color.new255(0, 255, 127),
    SpringGreen1:	Color.new255(0, 255, 127),
    SpringGreen2:	Color.new255(0, 238, 118),
    SpringGreen3:	Color.new255(0, 205, 102),
    SpringGreen4:	Color.new255(0, 139, 69),
    steel blue:	Color.new255(70, 130, 180),
    SteelBlue:	Color.new255(70, 130, 180),
    SteelBlue1:	Color.new255(99, 184, 255),
    SteelBlue2:	Color.new255(92, 172, 238),
    SteelBlue3:	Color.new255(79, 148, 205),
    SteelBlue4:	Color.new255(54, 100, 139),
    tan:	Color.new255(210, 180, 140),
    tan1:	Color.new255(255, 165, 79),
    tan2:	Color.new255(238, 154, 73),
    tan3:	Color.new255(205, 133, 63),
    tan4:	Color.new255(139, 90, 43),
    thistle:	Color.new255(216, 191, 216),
    thistle1:	Color.new255(255, 225, 255),
    thistle2:	Color.new255(238, 210, 238),
    thistle3:	Color.new255(205, 181, 205),
    thistle4:	Color.new255(139, 123, 139),
    tomato:	Color.new255(255, 99, 71),
    tomato1:	Color.new255(255, 99, 71),
    tomato2:	Color.new255(238, 92, 66),
    tomato3:	Color.new255(205, 79, 57),
    tomato4:	Color.new255(139, 54, 38),
    turquoise:	Color.new255(64, 224, 208),
    turquoise1:	Color.new255(0, 245, 255),
    turquoise2:	Color.new255(0, 229, 238),
    turquoise3:	Color.new255(0, 197, 205),
    turquoise4:	Color.new255(0, 134, 139),
    violet:	Color.new255(238, 130, 238),
    violet red:	Color.new255(208, 32, 144),
    VioletRed:	Color.new255(208, 32, 144),
    VioletRed1:	Color.new255(255, 62, 150),
    VioletRed2:	Color.new255(238, 58, 140),
    VioletRed3:	Color.new255(205, 50, 120),
    VioletRed4:	Color.new255(139, 34, 82),
    wheat:	Color.new255(245, 222, 179),
    wheat1:	Color.new255(255, 231, 186),
    wheat2:	Color.new255(238, 216, 174),
    wheat3:	Color.new255(205, 186, 150),
    wheat4:	Color.new255(139, 126, 102),
    white:	Color.new255(255, 255, 255),
    white smoke:	Color.new255(245, 245, 245),
    WhiteSmoke:	Color.new255(245, 245, 245),
    yellow:	Color.new255(255, 255, 0),
    yellow green:	Color.new255(154, 205, 50),
    yellow1:	Color.new255(255, 255, 0),
    yellow2:	Color.new255(238, 238, 0),
    yellow3:	Color.new255(205, 205, 0),
    yellow4:	Color.new255(139, 139, 0),
    YellowGreen:	Color.new255(154, 205, 50)
);

*/
