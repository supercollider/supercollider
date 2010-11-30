
// some things you can do with the language

Magnitude.dumpClassSubtree; // dump class tree of all subclasses of Magnitude

Object.dumpClassSubtree; // dump a list of all classes (except metaclasses)

Collection.dumpByteCodes('select'); // dump the byte codes for a method

UGen.dumpSubclassList

Object.browse  // examine methods interactively in a GUI (OSX)

Object.dumpFullInterface  // list all methods for the classhierarchically

Event.dumpMethodList  // list instance methods alphabetically

SinOsc.dumpMethodList  // list class methods alphabetically



(
// find all methods who have an argument name that starts with the letter g.
Class.allClasses.do({ arg aClass; 
	if (aClass.methods.notNil, {
		aClass.methods.do({ arg aMethod;
			aMethod.argNames.do({ arg aSymbol;
				if ( aSymbol.asString.at(0) == $g, {
					postln(
						aClass.name.asString ++ " " ++ 
						aMethod.name.asString ++ " " ++ 
						aSymbol.asString
					);
				});
			})
		})
	})
})
)

Class.allClasses.printAll; "" // print all the classes in SCClassLib

