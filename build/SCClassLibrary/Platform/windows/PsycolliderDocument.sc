/*
The BEGINNINGS of a Document class for psycollider.
Currently has limitations compared to other Document implementations,
doesn't connect to all of psyco's potential.
*/
PsycolliderDocument : Document {
	var <path;

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
		path.openWinTextFile;
	}
	
	*findHelpFile { |str|
		^Help.findHelpFile(str)
	}
}
             