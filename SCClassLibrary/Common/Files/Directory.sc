/*
 	Not working,
 	but see String-pathMatch
*/
/*
DirectoryEntry {
	var <name, <pathName, <isDir=false, <isVisible;
}

Directory {
	var <>pathName;
	*new { arg pathName;
		^super.new.pathName_(pathName);
	}

	numFiles {
		var numFiles = 0;
		this.scanFiles({ numFiles = numFiles + 1; });
		^numFiles
	}
	at { arg index;
		^this.prAt(DirectoryEntry.new, index);
	}

	scan { arg func;
		// scan all entries in this Directory
		var entry, index = 0;
		while ({
			entry = this.at(index);
			entry.notNil
		},{
			func.value(entry, index);
			index = index + 1;
		});
	}

	scanFiles { arg func;
		var index = 0;
		// scan only files in this Directory
		this.scan({ arg entry;
			if (entry.isDir.not and: { entry.isVisible }, {
				func.value(entry, index);
				index = index + 1;
			})
		});
	}

	deepScan { arg func;
		// recursively scan all files in and below this Directory
		this.scan({ arg entry;
			if (entry.isDir, {
				Directory.new(entry.pathName).scanFiles(func);
			},{
				func.value(entry);
			})
		});
	}

	// PRIVATE
	prAt { arg entry, index;
		_Directory_At
	}
}
*/
