/*
// Helper, ClassHelper, UGenHelper, TopicHelper
// (In this file: UGenHelper)
// ________________________________
//
// Classes to auto generate help files following style guides.
// See Helper source for more comments
// started on 04/11/07 - andreavalle
*/


UGenHelper {
	
	var <>class, <>path ;
	classvar doctype, head, preface, examples, parents ;
	
	*new { arg class, path ;
			^super.new.initUGenHelper( class, path ) 
	}
	
	initUGenHelper { arg aClass, aPath ;
		class = aClass ;
		path = aPath ;
		parents  = "" ;
		if ( path.isNil, {
			GUI.current.dialog.savePanel({ arg newPath ;
				 path = newPath ; {this.makeHelp}.defer })
			}, { this.makeHelp }) ;
	}
	
	createText {
		head = "
<head>
<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">
<meta http-equiv=\"Content-Style-Type\" content=\"text/css\">
<title></title>
<meta name=\"Generator\" content=\"Cocoa HTML Writer\">
<meta name=\"CocoaVersion\" content=\"824.42\">
<style type=\"text/css\">
p.p1 {margin: 0.0px 0.0px 0.0px 0.0px; font: 12.0px Helvetica}
p.p2 {margin: 0.0px 0.0px 0.0px 0.0px; font: 12.0px Helvetica; min-height: 14.0px}
p.p3 {margin: 0.0px 0.0px 0.0px 0.0px; font: 9.0px Monaco; min-height: 12.0px}
p.p4 {margin: 0.0px 0.0px 0.0px 0.0px; font: 9.0px Monaco; color: #d40000}
p.p5 {margin: 0.0px 0.0px 0.0px 0.0px; font: 9.0px Monaco}
p.p6 {margin: 0.0px 0.0px 0.0px 0.0px; font: 9.0px Monaco; color: #007700}
span.s1 {font: 18.0px Helvetica}
span.s2 {color: #1200c4}
span.s3 {color: #1200c4}
span.s4 {color: #000000}
span.s5 {color: #1200c4}
span.Apple-tab-span {white-space:pre}
</style>
</head>
" ;
		preface = "
<p class=\"p1\"><span class=\"s1\"><b>SomeUGen<span class=\"Apple-tab-span\">	</span><span class=\"Apple-tab-span\">	</span><span class=\"Apple-tab-span\">	</span></b></span><b>explanation of the what SomeUGen does</b></p>
<p class=\"p2\"><b></b><br></p>
"			.replace("SomeUGen", class.name.asString) ;
			doctype = "
<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">
" ;	
		examples = "
<p class=\"p4\">// what this example does</p>
<p class=\"p5\">x = { <span class=\"s3\">SomeUGen</span>.ar(<span class=\"s3\">XLine</span>.kr(2000, 200), 0, 0.5) }.play;</p>
<p class=\"p5\">x.free;</p>
<p class=\"p3\"><br></p>
<p class=\"p4\">// what this example does</p>
<p class=\"p5\">(</p>
<p class=\"p6\"><span class=\"s4\">x = </span><span class=\"s5\">SynthDef</span><span class=\"s4\">(</span>\\helpSomeUGen<span class=\"s4\">, {<span class=\"Apple-converted-space\"></span></span></p>
<p class=\"p5\"><span class=\"Apple-tab-span\"></span><span class=\"s3\">Out</span>.ar(0, <span class=\"s3\">SomeUGen</span>.ar(<span class=\"s3\">SinOsc</span>.ar(<span class=\"s3\">XLine</span>.kr(1, 1000, 9), 0, 200, 800), 0, 0.25));<span class=\"Apple-converted-space\"></span></p>
<p class=\"p5\">}).play;<span class=\"Apple-converted-space\"></span></p>
<p class=\"p5\">)</p>
<p class=\"p5\">x.free;</p>
"			.replace("SomeUGen", class.name.asString) ; 
	}


	makeHelp {
		var title = class.name.asString ;
		var classMethodBlock = this.createClassMethodBlock ;
		this.createText ;
		// see ClassHelper
		// write a html file in tmp
		File.new(path, "w")
			.write(doctype +
				 "<hmtl>\n"+
				 head+
				 "<body>\n"+
				 preface+
				 classMethodBlock + 
				 examples+
				 "</body>"+
				 "</html>")
				.close ;
		// and reopen thru class.openHelpFile
		// open works if the path is a place where SC looks for Help files
		class.openHelpFile 
	}

	createClassMethodBlock {
		var arArgs, krArgs, irArgs ;
		var arBlock = "", krBlock = "", irBlock = "" ;
		var explanation = "
<p class=\"p2\"><br></p>
<p class=\"p1\">Here is a short prose explanation of what the UGen does, and any other relevant points.</p>
" ;
		var total, defaults, default ;
		var par ;
		var uGen, uGenParent = class ; // we need to get the args
		var uGenParentMethods = uGenParent.class.methods.collect(_.name) ;
		var arMethod, krMethod, irMethod ;
		if ( uGenParentMethods == nil, { uGenParentMethods = [] }) ;
		// in the following we crawl on the tree untile we find ar/kr/ir methods
		// this works fine for most UGen
		// but not for the ones not implementing *r methods
		// in that case a default generic string is returned
		// The aim being to generate a template, not such a bad solution
		while { 
			( uGenParentMethods.includes(\ar) or:
			uGenParentMethods.includes(\kr) or:
			uGenParentMethods.includes(\ir)).not
		 } 
			{ uGen = uGenParent ; 
				// returns a "warning" but styled message
				// if *r methods are not found
				if (uGenParent == nil, {
				^"<p class=\"p1\"><b>"++
				class.name.asString+
				"-->no ar/kr/ir methods: sorry, you have to complete as required"+
				explanation+"</p></b>"
				} );	
			uGenParent = uGenParent.superclass ;
			 uGenParentMethods = uGenParent.class.methods.collect(_.name) ;
			 if ( uGenParentMethods == nil, { uGenParentMethods = [] }) ;
			 } ;

		arMethod = uGenParent.class.findMethod(\ar) ;
		krMethod = uGenParent.class.findMethod(\kr) ;
		irMethod = uGenParent.class.findMethod(\ir) ;
 		
 		defaults = arMethod ;
 		if ( defaults.isNil, { defaults = krMethod }) ;
 		if ( defaults.isNil, { defaults = irMethod }) ;
 		defaults = defaults.argumentString ;
 		defaults = defaults.split($,) ;
		arArgs = arMethod.argNames ; 
		krArgs = krMethod.argNames ; 
		irArgs = irMethod.argNames ; 		
		case 
		{ arArgs.size == 1 }{ 		
		arBlock = "
<p class=\"p1\"><b>SomeUGen.ar".replace("SomeUGen", class.name.asString) }
 
		{ arArgs.notNil} {
		 arBlock = "
<p class=\"p1\"><b>SomeUGen.ar(".replace("SomeUGen", class.name.asString) ;
		arArgs = arArgs[1..] ; 
		arArgs.do({ arg anArg ; 
			arBlock = arBlock++anArg.asString++", "
		}) ;
		arBlock = arBlock[..arBlock.size-3]+")</b></p>" ; 
		} ;
		
		case 
		{ krArgs.size == 1 }{ 		
		krBlock = "
<p class=\"p1\"><b>SomeUGen.kr".replace("SomeUGen", class.name.asString) }

		{ krArgs.notNil} {
		krBlock = "
<p class=\"p1\"><b>SomeUGen.kr(".replace("SomeUGen", class.name.asString) ;
		krArgs = krArgs[1..] ;
		krArgs.do({ arg anArg ; 
			krBlock = krBlock++anArg.asString++", "
		}) ;
		krBlock = krBlock[..krBlock.size-3]+")</b></p>" ; 
		} ;

		case 
		{ irArgs.size == 1 }{ 		
		irBlock = "
<p class=\"p1\"><b>SomeUGen.ir".replace("SomeUGen", class.name.asString) ;}
		
		{irArgs.notNil} {
		irBlock = "
<p class=\"p1\"><b>SomeUGen.ir(".replace("SomeUGen", class.name.asString) ;
		irArgs = irArgs[1..] ;
		irArgs.do({ arg anArg ; 
			irBlock = irBlock++anArg.asString++", "
		}) ;
		irBlock = irBlock[..irBlock.size-3]+")</b></p>" ; 
		} ;
		par = if (arArgs.notNil, { arArgs }, { if (krArgs.notNil, { krArgs }, { irArgs } ) }) ;
		//par = if ( par.size == 1, { [] }, { par }) ;
		// all those replacements are tricky, indeed
		// could be implemented much better, probably
		par.do({ arg anArg, index ; 
			default = defaults[index+1].asString ;
			default = if ( default.split($=).size == 1, { "nil" }, {
				 default.split($=)[1] });
			explanation = explanation +
"
<p class=\"p1\"><b>arg1</b> - explanation of arg1. Default value is defaultValue.</p>
"				.replace("arg1", anArg.asString)
				.replace("defaultValue", default) ; 

		}) ;
		explanation = explanation +
"
<p class=\"p2\"><br></p>
<p class=\"p1\"><b>See also:</b> <a href=\"../UGens/Oscillators/SinOsc.html\"><span class=\"s2\">SinOsc</span></a> [other related UGens...]</p>
<p class=\"p2\"><br></p>
<p class=\"p3\"><br></p>
" ;		
		total = arBlock+krBlock+irBlock+explanation ;
		^total
	}

	
}
//"