// a class for autogenerating help files from source markup
// a special multiline comment is used: /*@@*/
// plus some other conventions
// After class name you can write
/*@
shortDesc: description of the method
longDesc: longer one
seeAlso: refs
issues: other aspects
testvar: a name of a instance var
classtestvar: a name of a class var
instDesc: the name of the instance methods section
longInstDesc: a comment related to the instance methods section
@*/
//
// After each method you can write
/*@
desc: a description of the method
argName: for each arg. Default value is added automatically
ex: multiline example
@*/
// started after 04/11/07
// andreavalle

AutoClassHelper {

	var class, path, parser, classDoc, cmDict, imDict ;
	var shortDesc, longDesc, seeAlso, issues, instDesc, longInstDesc ;
	var varDict ;
	var doctype, head, preface, examples ;

	*new { arg undocumentedClass, path ;
			^super.new.initClassHelper( undocumentedClass, path )
	}

	initClassHelper { arg aClass, aPath ;
		class = aClass ;
		parser = DocParser.new(class) ;
		classDoc = parser.getDocs[0] ;
		cmDict = parser.getDocs[1] ;
		imDict = parser.getDocs[2] ;
		varDict = IdentityDictionary.new ;
		// <head> tag
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
span.s8 {color: #0000ff}
span.s9 {color: #1200c4}
span.Apple-tab-span {white-space:pre}
</style>
</head>
" ;
			doctype = "
<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">
" ;
		path = aPath ;
		if ( path.isNil, {
			GUI.current.dialog.savePanel({ arg newPath ;
				 path = newPath ; {this.makeHelp}.defer })
			}, { this.makeHelp }) ;

	}

	makeHelp {
		var title = class.name.asString ;
		var preface = this.createPreface ;
		var classMethodBlock = this.createClassMethodBlock ;
		var getterSetter = this.createGetterSetterBlock ;
		var instanceMethodBlock = this.createInstanceMethodBlock ;
		var doc, content ;
		// here we put together all the stuff
		content = doctype +
				 "<hmtl>\n"+
				 head+
				 "<body>\n"+
				 preface +
				 classMethodBlock +
				 getterSetter +
				instanceMethodBlock +
				// examples+
				 "</body>"+
				 "</html>" ;
		File.new(path, "w")
				.write(content)
				.close ;
		// and reopen thru class.openHelpFile
		// open works if the path is a place where SC looks for Help files
		class.openHelpFile
	}

	createPreface {
		var superclasses, parents = "" ;
		this.createClassDocBlock ;
		// here only for the special case of Object
		superclasses = if ( class == Object, { nil }, { class.superclasses.reverse }) ;
		superclasses.do({arg item ; parents = parents+item+":" }) ;
		^preface = "
<p class=\"p1\"><span class=\"s1\"><b>SomeClass<span class=\"Apple-tab-span\">	</span><span class=\"Apple-tab-span\">	</span><span class=\"Apple-tab-span\">	</span></b></span><b>shortDesc</b><span class=\"s1\"><b><span class=\"Apple-tab-span\">	</span></b></span></p>
<p class=\"p2\"><br></p>
<p class=\"p1\"><b>Inherits from: </b><b>Parents</b></p>
<p class=\"p3\"><br></p>
<p class=\"p1\">longDesc</p>
<p class=\"p2\"><br></p>
<p class=\"p1\"><b>See also:</b> seeAlso</p>
<p class=\"p2\"><br></p>
<p class=\"p4\"><b>Other Issues</b></p>
<p class=\"p2\"><br></p>
<p class=\"p1\">issues</a></p>
<p class=\"p2\"><br></p>
"			.replace("SomeClass", class.name.asString)
			.replace("Parents", parents[..parents.size-2])
			.replace("shortDesc", shortDesc)
			.replace("longDesc", longDesc)
			.replace("issues", issues)
			.replace("seeAlso", seeAlso)

	}

	createClassMethodBlock {
		var isEx, methodTitle ;
		var classMethods = "
<p class=\"p4\"><b>Creation / Class Methods</b></p>
<p class=\"p5\"><br></p>
" ;
		var method, name, args, def, txt, ex, default ;
		cmDict.keys.asArray.sort.do({ arg key ;
			isEx = false ;
			method = cmDict[key][0] ;
			name = method.name ;
			methodTitle = "
<p class=\"p6\"><b><span class=\"Apple-tab-span\">	</span>name</b></p>
"				.replace("name", "*"++name+this.makeName(method)) ;
			if ( cmDict[key][1] != "", {classMethods = classMethods + methodTitle }) ;
			args = cmDict[key][1].split($\n).reject({|i| i.size < 2}) ;
			args[..args.size-2].do({ arg line ;
				case	{ line.contains("desc:") }
						{ 	def = line.split($:)[0] ;
							txt = line.split($:)[1] ;
							classMethods = classMethods + "
<p class=\"p6\"><b><span class=\"Apple-tab-span\">	</span><span class=\"Apple-tab-span\">	</span></b>desc</p>
" 					.replace("desc", txt)
							 }
					{ line.contains("ex:") }
						{ 	isEx = true ;
							def = line.split($:)[0] ;
							txt = line.split($:)[1] ;
							classMethods = classMethods +
"
<p class=\"p7\"><span class=\"Apple-tab-span\">	</span><span class=\"Apple-tab-span\">	</span></p>
<p class=\"p9\"><span class=\"s4\"><span class=\"Apple-tab-span\">	</span><span class=\"Apple-tab-span\">	</span></span>// Example</p>
<p class=\"p10\"><span class=\"Apple-tab-span\">	</span><span class=\"Apple-tab-span\">	</span>ex</p>
"
 					.replace("ex", txt)
							}

					{ line.contains("desc:").not && isEx }
						{ 	classMethods = classMethods +
"<p class=\"p10\"><span class=\"Apple-tab-span\">	</span><span class=\"Apple-tab-span\">	</span>ex</p>"
 					.replace("ex", line)
							}

					{ line.contains("desc:").not }
						{
							def = line.split($:)[0] ;
							default = this.getDefault(method, def) ;
							txt = line.split($:)[1] ;
							classMethods = classMethods +
"
<p class=\"p8\"><span class=\"Apple-tab-span\">	</span><span class=\"Apple-tab-span\">	</span><b>def </b>- txt. Default value is dfv.</p>
" 							.replace("def", def)
							.replace("txt", txt).replace("dfv", default);

							}
			});
		}) ;
		^(classMethods+"<p class=\"p5\"><br></p>\n")

	}


	makeName { arg method ;
		var name = method.argumentString.replace("this, ", "").split($,)
			.collect({ arg i ; i.split($=)[0]}).asString ;
			^"("++name[2..name.size-3]++")"

	}

	getDefault { arg method, name ;
		var mString = method.argumentString.replace(" ", "") ;
		var pos ;
		var next ;
		name = name.reject( { arg i; i.isAlphaNum.not } ) ;
		pos = mString.find(name) ;
		if ( pos.notNil, {
			 next = mString[pos+name.size] ;
				if (next == $=, { mString = mString[pos+name.size..].split($=)[1] ;
								^mString = mString.split($,)[0] },
							{^"nil"})
			}, {^"nil"})
	}

	createInstanceMethodBlock {
		var isEx, methodTitle ;
		var instanceMethods = "<p class=\"p11\"><b>instDesc</b></p>
<p class=\"p12\"><br></p>
<p class=\"p6\">longInstDesc</p>
<p class=\"p7\"><span class=\"Apple-tab-span\">	</span></p>
" 			.replace("instDesc", instDesc)
			.replace("longInstDesc", longInstDesc)
			;
		var method, name, args, def, txt, ex, default ;
		imDict.keys.asArray.sort.do({ arg key ;
		 	isEx = false ;
			method = imDict[key][0] ;
			name = method.name ;
			methodTitle = "<p class=\"p5\"><br></p>\n"+
			"
<p class=\"p6\"><b><span class=\"Apple-tab-span\">	</span>name</b></p>
"				.replace("name", name++" "++this.makeName(method)) ;
			if ( imDict[key][1] != "", { instanceMethods = instanceMethods + methodTitle }) ;
			args = imDict[key][1].split($\n).reject({|i| i.size < 2}) ;
			args[..args.size-2].do({ arg line ;
				case	{ line.contains("desc:") }
						{ 	def = line.split($:)[0] ;
							txt = line.split($:)[1] ;
							instanceMethods = instanceMethods + "
<p class=\"p6\"><b><span class=\"Apple-tab-span\">	</span><span class=\"Apple-tab-span\">	</span></b>desc</p>
" 					.replace("desc", txt)
							 }
					{ line.contains("ex:") }
						{ 	isEx = true ;
							def = line.split($:)[0] ;
							txt = line.split($:)[1] ;
							instanceMethods = instanceMethods +
"
<p class=\"p7\"><span class=\"Apple-tab-span\">	</span><span class=\"Apple-tab-span\">	</span></p>
<p class=\"p9\"><span class=\"s4\"><span class=\"Apple-tab-span\">	</span><span class=\"Apple-tab-span\">	</span></span>// Example</p>
<p class=\"p10\"><span class=\"Apple-tab-span\">	</span><span class=\"Apple-tab-span\">	</span>ex</p>
"
 					.replace("ex", txt)
							}

					{ line.contains("desc:").not && isEx }
						{
							instanceMethods = instanceMethods +
"<p class=\"p10\"><span class=\"Apple-tab-span\">	</span><span class=\"Apple-tab-span\">	</span>ex</p>"
 					.replace("ex", line)
							}


					{ line.contains("desc:").not }
						{ 	def = line.split($:)[0] ;							default = this.getDefault(method, def) ;
							txt = line.split($:)[1] ;
							instanceMethods = instanceMethods +
"
<p class=\"p8\"><span class=\"Apple-tab-span\">	</span><span class=\"Apple-tab-span\">	</span><b>def </b>- txt. Default value is dfv.</p>
" 							.replace("def", def).replace("txt", txt).replace("dfv", default);

							}
			});
		}) ;
		^(instanceMethods+"<p class=\"p5\"><br></p>\n")
	}


	createClassDocBlock {
		var args = classDoc.split($\n).reject({|i| i.size < 2}) ;
		args.do({ arg line ;
			case	{ line.contains("shortDesc:") }
					{ shortDesc = line.split($:)[1] ; }
				{ line.contains("longDesc:") }
					{ longDesc = line.split($:)[1] ; }
				{ line.contains("seeAlso:") }
					{ seeAlso = line.split($:)[1] ; }
				{ line.contains("issues:") }
					{ issues = line.split($:)[1] ; }
				{ line.contains("instDesc:") }
					{ instDesc = line.split($:)[1] ; }
				{ line.contains("longInstDesc:") }
					{ longInstDesc = line.split($:)[1] ; }
				// residual: allows to create the varDict
				{ line.contains("longInstDesc:").not }
					{ varDict.add( line.split($:)[0]
						.select({|i| i.isAlphaNum })
						.asSymbol
						-> line.split($:)[1] ); }
			}) ;
	}


	createGetterSetterBlock {
		var getterSetterBlock = "
<p class=\"p11\"><b>Accessing Instance and Class Variables</b></p>
<p class=\"p7\"><span class=\"Apple-tab-span\">	</span></p>
" ;
		var getters = parser.getters.collect(_.name) ;
		var setters = parser.setters.collect(_.name) ;
		var name, comment, line ;
		varDict.keys.do({ arg key ;
			comment = varDict[key] ;
			line = "" ;
			line = if ( getters.includes(key), {
"
<p class=\"p6\"><b><span class=\"Apple-tab-span\">	</span>someVar</b></p>
"				.replace("someVar", key.asString)
			},{""}) ;
			if ( setters.includes((key++"_").asSymbol), {  line = line +
"
<p class=\"p6\"><b><span class=\"Apple-tab-span\">	</span>someVar_</b></p>
"				.replace("someVar", key.asString)
			}) ;
			if ( class.classVarNames.includes(key), {  line = line +
"
<p class=\"p6\"><b><span class=\"Apple-tab-span\">	</span>someVar</b></p>
"				.replace("someVar", key.asString)

			}) ;
			getterSetterBlock = getterSetterBlock + line +
"
<p class=\"p7\"><b><span class=\"Apple-tab-span\">	</span></b><span class=\"Apple-tab-span\">	</span></p>
<p class=\"p6\"><span class=\"Apple-tab-span\">	</span><span class=\"Apple-tab-span\">	</span>"+comment+"</p>
<p class=\"p7\"><span class=\"Apple-tab-span\">	</span><span class=\"Apple-tab-span\">	</span><span class=\"Apple-tab-span\">	</span><span class=\"Apple-tab-span\">	</span></p>
"
		}) ;
		^getterSetterBlock
	}

}

//"