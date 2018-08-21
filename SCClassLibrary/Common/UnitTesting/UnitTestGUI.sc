UnitTestGUI {


	*new { |allTestClasses|

		// UnitTest GUI written by Dan Stowell 2009.
		var w, classlist, methodlist, lookUp;

		w = Window.new("[UnitTest GUI]", Rect(100, 100, 415, 615), resizable: false);
		w.addFlowLayout;

		StaticText(w, Rect(0,0, 400, 40))
		.string_("Select a category, then a test method, and press Enter\nHit 'i' to jump to the code file")
		.align_(\center);

		classlist = ListView(w, Rect(0,0, 200, 600-40))
		.items_(allTestClasses.asSortedArray.collect(_[0]))
		.action_{|widg|
			methodlist.items_(
				allTestClasses.asSortedArray[widg.value][1].asSortedArray.collect(_[0])
			)
		};

		methodlist = ListView(w, Rect(200,40, 200, 600-40));
		methodlist.enterKeyAction_ {|widg|
			allTestClasses.asSortedArray[classlist.value][1].asSortedArray[widg.value][1].value
		};

		lookUp = {|widg, char, mod|
			var class, selector, method;
			class = ("Test" ++ classlist.items[classlist.value]).asSymbol.asClass;
			class !? {
				selector = methodlist.items[methodlist.value].asSymbol;
				method = class.findMethod(selector);
				if(char == $i) {
					if(method.notNil) { method.openCodeFile } { class.openCodeFile };
				}
			};

		};

		methodlist.keyDownAction = lookUp;
		classlist.keyDownAction = lookUp;

		classlist.enterKeyAction_{|widg|
			// mimic behaviour of pressing enter in methodlist
			methodlist.enterKey;
		};

		classlist.value_(0);
		classlist.doAction; // fills in the right-hand column
		^w.front;

	}

}
