// Required by AutoClassHelper
// Parses sources in search for comments
// andreavalle

DocParser {

	var <>targetClass ;
	var <classMethodList, <instMethodList, <getters, <setters ;
	var <file, <source ;


	*new {  arg class ;
		this.deprecated;
		^super.new.initDocParser(class) ;
	}

	initDocParser { arg aClass ;
		var instVarNames = aClass.instVarNames ;
		instVarNames = instVarNames ? [] ;
		targetClass = aClass ;
		classMethodList = targetClass.class.methods ;
		file = File.new(targetClass.filenameSymbol.asString, "r") ;
		source = file.readAllString ;
		file.close ;
		instMethodList = targetClass.methods ;
		instMethodList = instMethodList ? [] ;
		setters = instMethodList.reject({ arg item ; item.name.asString.includes($_).not }) ;
		getters = instMethodList.select({ arg item ;
			instVarNames.includes(item.name) }) ;
		setters = setters ? [] ; getters = getters ? [] ;
		instMethodList = instMethodList.reject({ arg item ;
			 getters.includes(item) || setters.includes(item) }) ;
		^this
	}

	createCmIndices {
		var cmIndices = [] ;
		classMethodList.do({ arg method ;
			cmIndices = cmIndices.add(method.charPos)
		}) ;
		^cmIndices
	}

	createImIndices {
		var imIndices = [] ;
		instMethodList.do({ arg method ;
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
		classMethodList.do({ arg method, i ;
			cmDict.add( i ->
				[method, this.getMethodDoc(method, cmIndices[i], cmIndices[i+1])]
			)
		}) ;
		instMethodList.do({ arg method, i ;
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
