// Required by AutoClassHelper
// Parses sources in search for comments
// andreavalle

DocParser {
	
	var <>class ;
	var <classMethods, <instMethods, <getters, <setters, <methods, <classMethods ;
	var <file, <source ;
	//var imDict, cmDict, classDoc ;
	
	
	*new {  arg class ;
		^super.new.initDocParser(class) ; 
	}

	initDocParser { arg aClass ;
		var instVarNames = aClass.instVarNames ;
		instVarNames = instVarNames ? [] ;
		class = aClass ;
		classMethods = class.class.methods ;
		file = File.new(class.filenameSymbol.asString, "r") ;
		source = file.readAllString ;
		file.close ;
		instMethods = class.methods ;
		instMethods = instMethods ? [] ;
		setters = instMethods.reject({ arg item ; item.name.asString.includes($_).not }) ;
		getters = instMethods.select({ arg item ; 
			instVarNames.includes(item.name) }) ;
		setters = setters ? [] ; getters = getters ? [] ;
		instMethods = instMethods.reject({ arg item ; 
			 getters.includes(item) || setters.includes(item) }) ;
/*
		classMethods.collect(_.name).postln ;
		classMethods.collect(_.name).postln ;
		setters.collect(_.name).postln ;
		getters.collect(_.name).postln ;
		instMethods.collect(_.name).postln ;
*/
		^this
	}
	
	createCmIndices {
		var cmIndices = [] ;
		classMethods.do({ arg method ; 
			cmIndices = cmIndices.add(method.charPos)
		}) ;
		^cmIndices
	}
	
	createImIndices {
		var imIndices = [] ;
		instMethods.do({ arg method ; 
			imIndices = imIndices.add(method.charPos)
		}) ;
		^imIndices
	}
	
	
	createGetSetIndices {
		var getSetIndices = [] ;
		getters.do({ arg method ; 
			getSetIndices = getSetIndices.add(method.charPos)
		}) ;
		setters.do({ arg method ; 
			getSetIndices = getSetIndices.add(method.charPos)
		}) ;
		^getSetIndices = getSetIndices.sort ;

	}
	
	getDocs {
		var imIndices = this.createImIndices ;
		var cmIndices = this.createCmIndices ;
		var getSetIndices = this.createGetSetIndices ;
		var total ;
		var imDict = IdentityDictionary.new ;
		var cmDict = IdentityDictionary.new ;
		var classDoc ; 
		cmIndices = cmIndices.add(imIndices[0]) ;
		imIndices =imIndices.add(source.size) ;
		classMethods.do({ arg method, i ;
			cmDict.add( i ->
				[method, this.getMethodDoc(method, cmIndices[i], cmIndices[i+1])]
			)
		}) ;
		instMethods.do({ arg method, i ;
			imDict.add( i ->
				[method, this.getMethodDoc(method, imIndices[i], imIndices[i+1])]
			)
		}) ;
		total = getSetIndices.addAll(cmIndices).addAll(imIndices).sort ;
		classDoc = this.getClassDoc(total[0]) ;		
		^[classDoc, cmDict, imDict]
	}
	
	getMethodDoc { arg method, start, stop ;
 		var block = source[start..stop] ;
 		var begin = block.find("/*@") ;
 		var end ;
 		if (begin.notNil, {
 			 end  = block.find("@*/") ; 
 			 ^block[begin+3..end-1] ;
 		 }, {^""})
		
	}

	getClassDoc { arg start ;
 		var begin = source.find("/*@") ;
 		var end ;
 		if (begin < start, {
 			 end  = source.find("@*/") ; 
 			 ^source[begin+3..end-1] ;
 		 }, {^""})
		
		
	}

}
