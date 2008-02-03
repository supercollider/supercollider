/*
// Helper, ClassHelper, UGenHelper, TopicHelper
// (In this file: ClassHelper)
// ________________________________
//
// Classes to auto generate help files following style guides.
// See Helper source for more comments
// started on 04/11/07 - andreavalle
*/


ClassHelper {
	
	var <>class, <>path ;
	classvar doctype, head, preface, examples, parents ;
	
	*new { arg class, path ;
			^super.new.initClassHelper( class, path ) 
	}
	
	initClassHelper { arg aClass, aPath ;
		var superclasses ;
		parents  = "" ;
		// here only for the special case of Object
		superclasses = if ( aClass == Object, { nil }, { aClass.superclasses.reverse }) ;
		superclasses.do({arg item ; parents = parents+item+":" }) ;
		class = aClass ;
		path = aPath ;
		if ( path.isNil, {
			GUI.current.dialog.savePanel({ arg newPath ;
				 path = newPath ; {this.makeHelp}.defer })
			}, { this.makeHelp }) ;
	}
	
	createText {
		// <head> tag
		head = "
<head>
<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">
<meta http-equiv=\"Content-Style-Type\" content=\"text/css\">
<title>SuperCollider Help -"++class.name.asString++"</title>
<meta name=\"Generator\" content=\"Cocoa HTML Writer\">
<meta name=\"CocoaVersion\" content=\"824.42\">
<style type=\"text/css\">
p.p1 {margin: 0.0px 0.0px 0.0px 0.0px; font: 12.0px Helvetica}
p.p2 {margin: 0.0px 0.0px 0.0px 0.0px; font: 12.0px Helvetica; min-height: 14.0px}
p.p3 {margin: 0.0px 0.0px 0.0px 0.0px; font: 9.0px Monaco; min-height: 12.0px}
p.p4 {margin: 0.0px 0.0px 0.0px 0.0px; font: 14.0px Helvetica}
p.p5 {margin: 0.0px 0.0px 0.0px 57.0px; text-indent: -57.0px; font: 9.0px Monaco; min-height: 12.0px}
p.p6 {margin: 0.0px 0.0px 0.0px 57.0px; text-indent: -57.0px; font: 12.0px Helvetica}
p.p7 {margin: 0.0px 0.0px 0.0px 57.0px; text-indent: -57.0px; font: 12.0px Helvetica; min-height: 14.0px}
p.p8 {margin: 0.0px 0.0px 0.0px 85.0px; text-indent: -85.0px; font: 12.0px Helvetica}
p.p9 {margin: 0.0px 0.0px 0.0px 57.0px; text-indent: -57.0px; font: 9.0px Monaco; color: #d40000}
p.p10 {margin: 0.0px 0.0px 0.0px 57.0px; text-indent: -57.0px; font: 9.0px Monaco}
p.p11 {margin: 0.0px 0.0px 0.0px 57.0px; text-indent: -57.0px; font: 14.0px Helvetica}
p.p12 {margin: 0.0px 0.0px 0.0px 57.0px; text-indent: -57.0px; font: 14.0px Helvetica; min-height: 17.0px}
p.p13 {margin: 0.0px 0.0px 0.0px 85.0px; text-indent: -85.0px; font: 12.0px Helvetica; min-height: 14.0px}
p.p14 {margin: 0.0px 0.0px 0.0px 0.0px; font: 9.0px Monaco; color: #d40000}
p.p15 {margin: 0.0px 0.0px 0.0px 0.0px; font: 9.0px Monaco}
span.s1 {font: 18.0px Helvetica}
span.s2 {color: #1200c4}
span.s3 {color: #1200c4}
span.s4 {color: #000000}
span.s5 {color: #1200c4}
span.s6 {color: #d40000}
span.s7 {font: 12.0px Helvetica; color: #000000}
span.s8 {color: #1200c4}
span.s9 {color: #1200c4}
span.Apple-tab-span {white-space:pre}
</style>
</head>
" ;
		preface = "
<p class=\"p1\"><span class=\"s1\"><b>SomeClass<span class=\"Apple-tab-span\">	</span><span class=\"Apple-tab-span\">	</span><span class=\"Apple-tab-span\">	</span></b></span><b>explanation of what SomeClass is and/or does</b><span class=\"s1\"><b><span class=\"Apple-tab-span\">	</span></b></span></p>
<p class=\"p2\"><br></p>
<p class=\"p1\"><b>Inherits from: </b><b>Parents</b></p>
<p class=\"p3\"><br></p>
<p class=\"p1\">More detailed prose description of SomeClass.</p>
<p class=\"p2\"><br></p>
<p class=\"p1\"><b>See also:</b> <a href=\"../Core/Object.html\"><span class=\"s2\">Object</span></a> [some other help files]</p>
<p class=\"p2\"><br></p>
<p class=\"p4\"><b>Some Important Issues Regarding SomeClass (optional)</b></p>
<p class=\"p2\"><br></p>
<p class=\"p1\">Explanation of the issues. For more information see <a href=\"../Core/Nil.html\"><span class=\"s3\">Nil</span></a> and [some other help files].</p>
<p class=\"p2\"><br></p>
"			.replace("SomeClass", class.name.asString)
			.replace("Parents", parents[..parents.size-2]) ;		
			doctype = "
<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">
" ;	
		examples = "
<p class=\"p11\"><b>Examples</b></p>
<p class=\"p3\"><br></p>
<p class=\"p14\">// what this example does</p>
<p class=\"p15\">e = <span class=\"s9\">SomeClass</span>.new;</p>
<p class=\"p15\">e.instanceMethod(someObject);</p>
<p class=\"p15\">e.cleanUp;</p>
<p class=\"p3\"><br></p>
<p class=\"p15\">...</p>
"			.replace("SomeClass", class.name.asString) ; // g is default SwingOSC server, hence e
	}

	makeHelp {
		var title = class.name.asString ;
		var classMethodBlock = this.createClassMethodBlock ;
		var getterSetter = this.createGetterSetterBlock ;
		var instanceMethodBlock = this.createInstanceMethodBlock ;
		var doc, content ;
		this.createText ;
		// here we put together all the stuff
		content = doctype +
				 "<hmtl>\n"+
				 head+
				 "<body>\n"+
				 preface+
				 classMethodBlock + 
				 getterSetter +
				 instanceMethodBlock +
				 examples+
				 "</body>"+
				 "</html>" ;
		// when a path is provided, the file is written there
		File.new(path, "w")
				.write(content)
				.close ;
		// and reopen thru class.openHelpFile
		// open works if the path is a place where SC looks for Help files
		class.openHelpFile 
	}

	
	
	createClassMethodBlock {
		var classMethods = "
<p class=\"p4\"><b>Creation / Class Methods</b></p>
<p class=\"p5\"><br></p>
" ;
		var methods = class.class.methods.collect(_.name) ;
		var methodBlock = "" ;
		methods.do({ arg m ;  
			methodBlock = methodBlock+ 
			this.createMethod([m, 
				class.class.findMethod(m).argNames,
				 class.class.findMethod(m).argumentString 
				]) ;
		 }) ;
		^(classMethods+methodBlock) ;
	}


	// replicated
	createInstanceMethodBlock {
		var instanceMethods = "<p class=\"p11\"><b>Doing Some Task (optional)</b></p>
<p class=\"p12\"><br></p>
<p class=\"p6\">A short bit of prose explaining something about the task.</p>
<p class=\"p7\"><span class=\"Apple-tab-span\">	</span></p>
" ;
		var methods = class.methods.collect(_.name) ;
		var methodBlock = "" ;
		methods.do({ arg m ;  
			methodBlock = methodBlock+ 
			this.createMethod([m, 
						class.findMethod(m).argNames, 
						class.findMethod(m).argumentString], 
						true) ;
		 }) ;
		^(instanceMethods+methodBlock) ;
	}

// this generates the (class/instance) method block

	createMethod { arg methodArr, instance = false ;
		var methodBlock, argsBlock, defaultValue ;
		var name = methodArr[0].asString ;
		var example = "
<p class=\"p7\"><span class=\"Apple-tab-span\">	</span><span class=\"Apple-tab-span\">	</span></p>
<p class=\"p9\"><span class=\"s4\"><span class=\"Apple-tab-span\">	</span><span class=\"Apple-tab-span\">	</span></span>// inline example</p>
<p class=\"p10\"><span class=\"Apple-tab-span\">	</span><span class=\"Apple-tab-span\">	</span>g = <span class=\"s5\">SomeClass</span>.new;</p>
<p class=\"p10\"><span class=\"Apple-tab-span\">	</span><span class=\"Apple-tab-span\">	</span>g.doSomething; <span class=\"s6\">// explanation</span></p>
<p class=\"p10\"><span class=\"Apple-tab-span\">	</span><span class=\"Apple-tab-span\">	</span>g.cleanUpMethod;</p>
<p class=\"p5\"><br></p>
"		.replace("SomeClass", class.name).replace("new", name) ;
		methodBlock = "
<p class=\"p6\"><b><span class=\"Apple-tab-span\">	</span>*new(arg1, arg2)</b></p>
<p class=\"p7\"><b><span class=\"Apple-tab-span\">	</span></b></p>
<p class=\"p6\"><b><span class=\"Apple-tab-span\">	</span><span class=\"Apple-tab-span\">	</span></b>Short prose description of method.</p>
" ;
		if	( methodArr[1].class != SymbolArray, { ^""} ) ;	// this creates the name line
		if (methodArr[1].size >1, {
			name = name+"(" ;
			methodArr[1][1..].do({ arg anArg ; name = name++anArg.asString++", " ;
			}) ;
			name = name[..name.size-3]++")" ; // just to skip comma
			}) ;
		if (instance == false, { name = "*"++name }) ; // class or instance?
		// this adds args
		methodBlock = methodBlock.replace("*new(arg1, arg2)", name) ;
		argsBlock = "
<p class=\"p8\"><span class=\"Apple-tab-span\">	</span><span class=\"Apple-tab-span\">	</span><b>arg1 </b>- Explanation of arg1. Default value is argDefault. Other information.</p>
" ;
		// OK, the following is tricky
		methodArr[1][1..].do({ arg anArg, index ; 
			defaultValue = methodArr[2].split($,)[index+1].split($=) ; 
			defaultValue = if (defaultValue.size == 1, { "nil" }, { 
				defaultValue[1] }) ;
			methodBlock = methodBlock+argsBlock
				.replace("arg1", anArg)
				.replace( "argDefault", defaultValue) ; 
		}) ;
		^methodBlock +
			example +
			"<p class=\"p5\"><br></p>\n" 	
	} 


	createGetterSetterBlock {
		var getterSetter ;
		var getterSetterBlock = "
<p class=\"p11\"><b>Accessing Instance and Class Variables</b></p>
<p class=\"p7\"><span class=\"Apple-tab-span\">	</span></p>
" ;
		var methods = class.methods
			.collect(_.name)
			.reject({ arg item ;
				class.findMethod(item).argNames.class == SymbolArray}) ;
		var getter = methods.reject({ arg item ; item.asString.includes($_) }) ;
		var setter = methods.reject({ arg item ; item.asString.includes($_).not }) ;
		var names = methods.collect({ arg item ; item.asString.split($_)[0] }) ;
		var methodBlock = "" ;
		var defaultValue = "nil" ;
		if (names.notNil, { names = names.asSet.asArray } ) ;
		
		class.classVarNames.do({ arg clvar ;
				getterSetterBlock = getterSetterBlock + 
"	
<p class=\"p6\"><b><span class=\"Apple-tab-span\">	</span>someVar</b></p>
".replace("someVar", clvar.asString)+
"
<p class=\"p7\"><b><span class=\"Apple-tab-span\">	</span></b><span class=\"Apple-tab-span\">	</span></p>
<p class=\"p6\"><span class=\"Apple-tab-span\">	</span><span class=\"Apple-tab-span\">	</span>Explanation including the type of <b>someVar </b>and a link to its help file.</p>
<p class=\"p6\"><span class=\"Apple-tab-span\">	</span><span class=\"Apple-tab-span\">	</span>Default value is defaultValue.</p>
<p class=\"p7\"><span class=\"Apple-tab-span\">	</span><span class=\"Apple-tab-span\">	</span><span class=\"Apple-tab-span\">	</span><span class=\"Apple-tab-span\">	</span></p>
"		.replace("someVar", clvar.asString)
			});
		
		names.do({ arg m ;  
			case 
				{ getter.includes(m.asSymbol) and: setter.includes((m++"_").asSymbol) }
					{  
					defaultValue = class.findMethod(m.asSymbol).argumentString ;
					getterSetter = 
"
<p class=\"p6\"><b><span class=\"Apple-tab-span\">	</span>someVar_(arg1)</b></p>
<p class=\"p6\"><b><span class=\"Apple-tab-span\">	</span>someVar</b></p>
"					
					}
				{ getter.includes(m.asSymbol) and: setter.includes((m++"_").asSymbol).not }
					{ 
					defaultValue = class.findMethod(m.asSymbol).argumentString ;
					getterSetter =
"
<p class=\"p6\"><b><span class=\"Apple-tab-span\">	</span>someVar</b></p>
"
					}
				{ getter.includes(m.asSymbol).not and: setter.includes((m++"_").asSymbol) }
					{
					defaultValue = class.findMethod((m++"_").asSymbol).argumentString ;					getterSetter =
"
<p class=\"p6\"><b><span class=\"Apple-tab-span\">	</span>someVar_(arg1)</b></p>
"					
					} ;	
			if ( defaultValue.isNil, { defaultValue = "nil" }) ;
			methodBlock = methodBlock+ 
						getterSetter.replace("someVar", m.asString) +
"
<p class=\"p7\"><b><span class=\"Apple-tab-span\">	</span></b><span class=\"Apple-tab-span\">	</span></p>
<p class=\"p6\"><span class=\"Apple-tab-span\">	</span><span class=\"Apple-tab-span\">	</span>Explanation including the type of <b>someVar </b>and a link to its help file.</p>
<p class=\"p6\"><span class=\"Apple-tab-span\">	</span><span class=\"Apple-tab-span\">	</span>Default value is defaultValue.</p>
<p class=\"p7\"><span class=\"Apple-tab-span\">	</span><span class=\"Apple-tab-span\">	</span><span class=\"Apple-tab-span\">	</span><span class=\"Apple-tab-span\">	</span></p>
"		.replace("someVar", m.asString)
		.replace("defaultValue", defaultValue)
		 }) ;
		^(getterSetterBlock+methodBlock+
"
<p class=\"p7\"><span class=\"Apple-tab-span\">	</span><span class=\"Apple-tab-span\">	</span><span class=\"Apple-tab-span\">	</span><span class=\"Apple-tab-span\">	</span></p>
"		
		) ;
	}
	
}

//"