/*
The BEGINNINGS of a Document class for psycollider.
Currently has limitations compared to other Document implementations,
doesn't connect to all of psyco's potential.
*/
PsycolliderDocument : Document {
	var <path, 
		<id; // Should be the "windowId" used in psycollider to uniquely identify the window

	*initClass{
		Document.implementationClass = PsycolliderDocument;
	}
	
	*new { arg title="Untitled", string="", makeListener=false;
		^super.prBasicNew.initByString(title, string.asString, makeListener);
	}
	initByString { arg title="Untitled", string="", makeListener=false;
		var tempFile;
		path = "temp_newTextWindow";
		tempFile = File(path, "w");
		tempFile.write(string); 
		tempFile.close;
		id = path.openWinTextFile;
	}
	
	*findHelpFile { |str|
		^Help.findHelpFile(str)
	}

	*postColor_ {
	}
}
