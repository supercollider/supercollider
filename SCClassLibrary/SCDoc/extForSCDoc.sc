// the methods that implement cmd-j,y, shift-y are defined in Process and grab interpreter.cmdLine directly
// Here are alternate versions for String we use below
// possibly the Process methods shold be redefines to grab the cmdline and then call the string method...
+String {
	openCodeFile { 
		var class, method, words, string;
		string = this.copy;
		if (string.includes($:), {
			string.removeAllSuchThat(_.isSpace);
			words = string.delimit({ arg c; c == $: });
			class = words.at(0).asSymbol.asClass;
			if (class.notNil, {
				method = class.findMethod(words.at(1).asSymbol);
				if (method.notNil, {
					method.filenameSymbol.asString.openTextFile(method.charPos, -1);
				});
			});
		},{
			class = string.asSymbol.asClass;
			if (class.notNil, {
				class = class.classRedirect;
				class.filenameSymbol.asString.openTextFile(class.charPos, -1);
			});
		});
	}

	openWinCodeFile {
		var class, method, words, string;
		string = this.copy;
		if (string.includes($:), {
			string.removeAllSuchThat(_.isSpace);
			words = string.delimit({ arg c; c == $: });
			class = words.at(0).asSymbol.asClass;
			if (class.notNil, {
				method = class.findMethod(words.at(1).asSymbol);
				if (method.notNil, {
					method.filenameSymbol.asString.openWinTextFile(method.charPos, -1);
				});
			});
		},{
			class = string.asSymbol.asClass;
			if (class.notNil, {
				class = class.classRedirect;
				class.filenameSymbol.asString.openWinTextFile(class.charPos, -1);
			});
		});
	}


	methodReferences { 
		// this will not find method calls that are compiled with special byte codes such as 'value'.
		var out, references, name;
		name = this.asSymbol;
		out = CollStream.new;
		references = Class.findAllReferences(name);
		if (references.notNil, {
			out << "References to '" << name << "' :\n";
			references.do({ arg ref; out << "   " << ref.asString << "\n"; });
			out.collection.newTextWindow(name.asString);
		},{
			Post << "\nNo references to '" << name << "'.\n";
		});
	}
	methodTemplates { 
		// this constructs the method templates when cmd-Y is pressed in the Lang menu.
		var name, out, found = 0, namestring;
		name = this.copy;
		out = CollStream.new;

		if (name.isEmpty || name[0].isUpper){
			Post << "\nNo implementations of ''.\n";
			^this
		};
		if (name[0].toLower != name[0]) {
			// user pressed the wrong key. DWIM.
			^this.openCodeFile;
		};
		name = name.asSymbol;
		out << "Implementations of '" << name << "' :\n";
		Class.allClasses.do({ arg class;
			class.methods.do({ arg method;
				if (method.name == name, {
					found = found + 1;
					namestring = class.name ++ ":" ++ name;
					out << "   [" << namestring << "] :     ";
					if (method.argNames.isNil or: { method.argNames.size == 1 }, {
						out << "this." << name;
						if (name.isSetter, { out << "(val)"; });
					},{
						out << method.argNames.at(0);
						if (name.asString.at(0).isAlpha, {
							out << "." << name << "(";
							method.argNames.do({ arg argName, i;
								if (i > 0, {
									if (i != 1, { out << ", " });
									out << argName;
								});
							});
							out << ")";
						},{
							out << " " << name << " ";
							out << method.argNames.at(1);
						});
					});
					out.nl;
				});
			});
		});
		case
		{ found == 0 }
		{
			Post << "\nNo implementations of '" << name << "'.\n";
		}
		{ found == 1 }
		{
			this.openCodeFile(namestring);
		}
		{
			out.collection.newTextWindow(name.asString);
		};
	}
	
}

+QHelpBrowser {
	initQHelpBrowser {
		  keyDownAction = this.defaultKeyDownAction;
	  }
	 defaultKeyDownAction  { 
		  ^{ | v, char, mod, ascii, unicode | 
			var str, class;
			if (mod &0x1e000000 != 0) {	// do this if any of cmd, option,ctrl, or shift are pressed
				str = this.getProperty(\selectedString);
				if (str.size > 0) {
			 		switch(unicode)
		 		
						{ 16777220 } 		// cr
							{ str.interpret.postln  }
						{68}				// d
							{ //str.openHelpFile 
								QHelpBrowser.new
								.front
								.load(str.findHelpFile ? "Help/Help.html".standardizePath)
								
							}
						{74} {	// j
							class = str.asSymbol.asClass.postln;
								if (class.notNil) {
									class.openCodeFile; }
							}
							{89} {		//y 
								if (str.contains($:)) 
									{ str = str.split($:) } 
									{ str = str.split($.) };
								methodTemplates(str.last)
							}
							{85} 		//u
								{ if (str.contains($:)) 
									{ str = str.split($:) } 
									{ str = str.split($.) };
								methodReferences(str.last);
							}
				 }
			}
		}
	  }
}
