// TestCompilerBrutal.sc
// Brian Heim, 2017-02-28

TestCompilerBrutal : AbstractLPCBrutalTest {

	outputDir { ^"compiler/"; }

	// set to TRUE if you need `_expected` files
	makingValidationFiles { ^false; }

	evaluationTechnique { ^\bytecode }

	getAlphabets {
		// Note: these lists are based on information in PyrParseNode::initSpecialSelectors() and
		// Opcodes.h. This preprocessor function is a useful tool here. Modify it for your needs:
		//   thisProcess.interpreter.preProcessor = {|str| str.quote++".compile.def.dumpopcodes"}
		// -Brian
		var wordOps = [
			"neg",
			"reciprocal",
			"not",
			"isNil",
			"notNil",
			"bitNot",
			"abs",
			"asFloat",
			"asInt",
			"ceil",
			"floor",
			"frac",
			"sign",
			"squared",
			"cubed",
			"sqrt",
			"exp",
			"midicps",
			"cpsmidi",
			"midiratio",
			"ratiomidi",
			"ampdb",
			"dbamp",
			"octcps",
			"cpsoct",
			"log",
			"log2",
			"log10",
			"sin",
			"cos",
			"tan",
			"asin",
			"acos",
			"atan",
			"sinh",
			"cosh",
			"tanh",
			"rand",
			"rand2",
			"linrand",
			"bilinrand",
			"sum3rand",
			"distort",
			"softclip",
			"coin",
			"rectWindow",
			"hanWindow",
			"welWindow",
			"triWindow",
			"scurve",
			"ramp",
			"digitValue",
			"silence",
			"thru",
			"div",
			"mod",
			"min",
			"max",
			"bitAnd",
			"bitOr",
			"bitXor",
			"lcm",
			"gcd",
			"round",
			"roundUp",
			"trunc",
			"atan2",
			"hypot",
			"hypotApx",
			"pow",
			"leftShift",
			"rightShift",
			"unsignedRightShift",
			"fill",
			"ring1",
			"ring2",
			"ring3",
			"ring4",
			"difsqr",
			"sumsqr",
			"sqrsum",
			"sqrdif",
			"absdif",
			"thresh",
			"amclip",
			"scaleneg",
			"clip2",
			"fold2",
			"wrap2",
			"excess",
			"firstArg",
			"rrand",
			"exprand",
			"new",
			"newClear",
			"newCopyArgs",
			"init",
			"at",
			"put",
			"next",
			"reset",
			"value",
			"copyToEnd",
			"size",
			"class",
			"if",
			"while",
			"for",
			"and",
			"or",
			"case",
			"switch",
			"print",
			"add",
			"remove",
			"indexOf",
			"wrapAt",
			"clipAt",
			"foldAt",
			"wrapPut",
			"clipPut",
			"foldPut",
			"do",
			"collect",
			"select",
			"reject",
			"any",
			"every",
			"find",
			"choose",
			"valueList",
			"addFirst",
			"primitiveFailed",
			"subclassResponsibility",
			"shouldNotImplement",
			"doesNotUnderstand",
			"notYetImplemented",
			"multiNew",
			"multiNewList",
			"ar",
			"kr",
			"ir",
			"envirGet",
			"envirPut",
			"halt",
			"forBy",
			"forSeries",
			"reverseDo",
			"loop",
			"mustBeBoolean",
			"copy",
			"performList",
			"isKindOf",
			"postln",
			"asString",
			"yield",
			"name",
			"madd",
			"series"
		];

		var symbolOps = [
			"+",
			"-",
			"*",
			"/",
			"==",
			"!=",
			"<",
			">",
			"<=",
			">=",
			"===",
			"!==",
			"@",
			"@@",
			"|@|",
			"@|@",
			"++",
			"<<",
			"?",
			"??",
			"!?"
		];

		var literals = [
			"\\sym",
			"\"str\"",
			"$c",

			// special values — see again PyrParseNode.cpp and Opcodes.h
			"-1",
			"0",
			"1",
			"2",
			"-1.0",
			"0.0",
			"0.5",
			"1.0",
			"2.0"
		];

		var keywords = [
			"true",
			"false",
			"nil",
			"inf",
			"this",
			"super",

			// pseudo-variables
			"thisProcess",
			"thisMethod",
			"thisFunction",
			"thisFunctionDef",
			"thisThread"
		];

		// special classes — see again PyrParseNode.cpp and Opcodes.h
		var classNames = [
			"Object",
			"Symbol",
			"Nil",
			"Boolean",
			"True",
			"False",
			"Magnitude",
			"Char",
			"Number",
			"Complex",
			"Integer",
			"Float",
			"Method",
			"FunctionDef",
			"Stream",
			"Function",
			"Frame",
			"Process",
			"Main",
			"Class",
			"String",
			"Collection",
			"SequenceableCollection",
			"ArrayedCollection",
			"Array",
			"Int8Array",
			"Int16Array",
			"Int32Array",
			"FloatArray",
			"Signal",
			"DoubleArray",
			"SymbolArray",
			"List",
			"LinkedList",
			"Bag",
			"Set",
			"IdentitySet",
			"Dictionary",
			"IdentityDictionary",
			"SortedList",
			"Synth",
			"Ref",
			"Environment",
			"Event",
			"Wavetable",
			"Env",
			"Routine",
			"Color",
			"Rect"
		];

		// These are all control structure words; some of them may not
		// actually call on the compiler for special treatment, but by
		// including them here we are being thorough.
		var controlOps = [
			"if",
			"while",
			"switch",
			"case",
			"do",
			"for",
			"forBy"
		];

		// Some unique statements that cover additional opcodes emitted
		// by the compiler. Feel free to expand this list.
		var uniqueStatements = [
			// return true, false, nil, true
			"^",
			"^nil",
			"^false",
			"^true",

			// add 1, subtract 1
			"1+1",
			"1-1",

			// other random statements
			"a=3",
			"var def; def=3;",
			"super.a"
		];

		^Dictionary[
			// All operators put into infix form. Word operators
			// need to be postfixed with `:` to become method selectors.
			\infix -> (wordOps.collect(_++":") ++ symbolOps),

			\wordOps -> wordOps,

			\symbolOps -> symbolOps,

			\literals -> literals,

			\keywords -> keywords,

			\classes -> classNames,

			\control -> controlOps,

			\unique -> uniqueStatements,

			// all ASCII values except 0 `NUL` and 7F `DEL`
			\allChars -> (1..126).collect({ arg c; c.asAscii.asString })
		];
	}

	getStringLengthsPerAlphabet {
		// allChars should be tested at lengths up to 3.
		// All others use string length one, essentially simulating
		// string formatting.
		^stringLengthsPerAlphabet ? Dictionary[
			\infix -> [1],
			\wordOps -> [1],
			\symbolOps -> [1],
			\literals -> [1],
			\keywords -> [1],
			\classes -> [1],
			\control -> [1],
			\unique -> [1],

			\allChars -> [1,2,3]
		];
	}

	// tests for infix alphabet
	test_infix_basic { this.runTestsTogglingTCO("a ", " b", "basic", \infix); }
	test_infix_vars { this.runTestsTogglingTCO("var lh, rh; lh ", " rh", "vardefs", \infix); }

	// tests for wordOps alphabet
	test_wordOps_oneArg { this.runTestsTogglingTCO("a.", "(b)", "oneArg", \wordOps); }
	test_wordOps_twoArgs { this.runTestsTogglingTCO("a.", "(b, c)", "twoArgs", \wordOps); }

	// tests for symbolOps alphabet
	test_symbolOps_adverbs { this.runTestsTogglingTCO("a ", ".t b", "adverbs", \symbolOps); }

	// tests for literals alphabet
	test_literals_basic { this.runTestsTogglingTCO("", "", "basic", \literals); }

	// tests for keywords alphabet
	test_keywords_basic { this.runTestsTogglingTCO("", "", "basic", \keywords); }

	// tests for classes alphabet
	test_classes_basic { this.runTestsTogglingTCO("", "", "basic", \classes); }

	// tests for control alphabet
	test_control_oneArg { this.runTestsTogglingTCO("", "(a)", "oneArg", \control); }
	test_control_twoArgs { this.runTestsTogglingTCO("", "(a, b)", "twoArgs", \control); }
	test_control_threeArgs { this.runTestsTogglingTCO("", "(a, b, c)", "threeArgs", \control); }

	test_control_twoFuncArgs {
		this.runTestsTogglingTCO("", "({a.post}, {b.post})", "twoFuncArgs", \control);
	}

	test_control_threeFuncArgs {
		this.runTestsTogglingTCO("", "({a.post}, {b.post}, {c.post})", "threeFuncArgs", \control);
	}

	// tests for unique statements
	test_unique_basic { this.runTestsTogglingTCO("", "", "basic", \unique); }

	// tests for allChars alphabet
	test_allChars_basic { this.runTestsTogglingTCO("", "", "basic", \allChars); }
	test_allChars_infix { this.runTestsTogglingTCO("a ", " b", "infix", \allChars); }

}
