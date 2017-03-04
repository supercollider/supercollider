// TestCompilerBrutal.sc
// Brian Heim, 2017-02-28

TestCompilerBrutal : AbstractBrutalTest {

	outputDir { ^"valid_compiler/"; }

	// set to TRUE if you need `_expected` files
	makingValidationFiles { ^true; }

	initAlphabets {

		// TODO
		// Alphabets ought to make use of every possible opcode the interpreter
		// is capable of emitting while parsing cmd line.

		// Note: these lists are based on information in PyrParseNode::initSpecialSelectors()
		// and Opcodes.h. A useful tool I found is
		// this preprocessor function:
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

		var classNames = [
			// special classes — see again PyrParseNode.cpp and Opcodes.h
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

		var controlOps = [
			"if",
			"while",
			"switch",
			"case",
			"do",
			"for",
			"forBy"
		];

		alphabets = Dictionary[
			// All operators put into infix form. Word operators
			// need to be postfixed with `:` to become method selectors.
			\infix -> (wordOps.collect(_++":") ++ symbolOps),

			\wordOps -> wordOps,

			\symbolOps -> symbolOps,

			\literals -> literals,

			\keywords -> keywords,

			\classes -> classNames,

			\control -> controlOps,

			// all ASCII values except 0 `NUL` and 7F `DEL`
			\allChars -> (1..126).collect({ arg c; c.asAscii.asString })
		];

		// allChars should be tested at lengths up to 3.
		// All others use string length one, essentially simulating
		// string formatting.
		alphabetStringLengths = Dictionary[
			\infix -> [1],
			\wordOps -> [1],
			\symbolOps -> [1],
			\literals -> [1],
			\keywords -> [1],
			\classes -> [1],
			\control -> [1],

			\allChars -> [1,2,3]
		];
	}

	runCompilerTests {
		// TODO
	}

	test_basic {
		// TODO
	}

}