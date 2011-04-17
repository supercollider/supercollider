+ Process{
//this called when the menu item file-new or file-open is called
	addDocument {
		Document.prGetLast;
	}
}

+ Class {
	browse {
		^ClassBrowser.new(this)
	}
}
