// a class testing autodoc. You will see some naming conventions and a special comment block
// adding @ to multiline comment
// In the following we try to test all the possible situations

/*
You can use multiline comments here
*/

AutoClassHelperTest {
	
	// this is a fake class indeed
	
	/*@
	shortDesc: A class to test automatic documentation from source
	longDesc: The only limit of this tag is that it cannot break a line. This can be cumbersone, indeed. You can use <br> blocks. You will see that you have to specify in the source what you want to document. Note in any case that for specified args in method the default value is extracted automatically. Works with the Class template (no sense for UGens). 
	seeAlso: Homunculus, Leviathan
	issues: Caution! Frailty thy name is AutoClassHelper. You can omit tags but in order to have it running you cannot change their order. More, it works only for the first class defined in a source file. Note also that actually it assumes that you have one class for sourcefile.  It crashes if there's no class desc block. If method block descriptions are not indented as demonstrated they are not recognized properly. <br> Please use conventions specified in this class source file. 
	testvar: nothing special
	classtestvar: really interesting
	instDesc: How to become an anthropogenist
	longInstDesc: It's easy. Consider using the following methods.
	@*/

	var <>testvar, <another, >one, last ;
	// if you like to comment here, you can
	classvar classtestvar ;
	
	*new { arg ens = "Adam"; 
		
		// if you like to comment here, you can
		// for each method you can (optionally) provide
		// desc: a description line
		// arg: each argument
		// ex: a (multiline) example
		
		/*@
		desc: creation is typically reserved to God. Args follows.
		ens: it's a good start
		ex: 
		// a multiline example is possible
		// assuming that ex tag is the LAST one
		// a pity: no colored syntax
		TestClass.new("Eve").postln ; // comment
		TestClass.class.postln ;
		k = \aSymbol ;
		j = "a string" ;
		@*/
		// if you like to comment here, you can
		^super.new.init(ens);
	}

	*open { arg ensPattern ; 
		// nothing should appear in docs
		"creating from a mould".postln ;
	}

	init { arg testArg ;
		// here we omit description
		^testArg ;
	}

/*
Maybe you want to separate blocks of source code
with multiline.
You can, indeed
*/

// start working stuff 

	generate { arg fromScratch = true, deus ;
		/*@
		fromScratch: whether you're a real constructivist or not
		deus: sive natura (as you prefer)
		@*/
		"I like test".postln ;
	}

	destroy { arg ens, method = \evaporate ;
		/*@
		desc: it's a dirty job, but someone's got to do it
		method: there are many options. Quite efficient implementation
		@*/
		^nil
	}

	makeBadKarma { arg ens, toMineral = true ;
		/*@
		desc: "you can't always get what you want"
		toMineral: back to the roots
		ex:
		k = AutoDocTestClass.new ;
		k.makeBadKarma(toMineral: false) ;
		@*/
		^nil
	}

// stop real working stuff

}


// only the first class in a source file can be documented. 
// If you ask for autodoc on AutoDocTestClass2, e.g.
// 		AutoClassHelper(AutoDocTestClass2)
// SC raises an error.
// If AutoDocTestClass2 is in a separate file, all is ok. 
// Note that at least you need a /*@@*/ block in the class you need to document,
// otherwise SC raises an error 
// (well, does not make sense to autogenerate ah help file from no provided infos)


AutoDocTestClass2 {
	
	
	print { arg first, second ;
		[first, second].postln ;
	} 
	
	test { arg aTest ;
		/*@
		desc: "Saigon, shit, I'm still only in Saigon"
		@*/
		aTest.postln ;
	}
	
	
}