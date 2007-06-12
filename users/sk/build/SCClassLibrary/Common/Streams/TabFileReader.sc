FileReader {
	// a class to read text files automatically
	classvar <delim = $ ;	// space separated by default
	var stream;
	
	*new { arg stream;
		^super.newCopyArgs(stream)
	}
	
	*read { arg path, skipEmptyLines=false, skipBlanks=false, func, delimiter; 
		var f, table; 
		f = File(path, "r"); 
		if (f.isOpen.not) { warn("TabFileReader: file" + path + "not found.") ^nil };
		table = this.new(f).read(skipEmptyLines, skipBlanks, func, (delimiter));
		f.close;
		^table;
	}
	read { arg skipEmptyLines=false, skipBlanks=false, func, delimiter; 
		var string, record, table, c;

		delimiter = delimiter ? this.class.delim;
		string = String.new;

		while ({
			c = stream.getChar;
			c.notNil
		},{
			if (c == delimiter, {
				if (skipBlanks.not or: { string.size > 0 }) {
					func !? { string = func.value(string) };
					record = record.add(string);
					string = String.new;
				}
			},{
			if (c == $\n or: { c == $\r }, {
				func !? { string = func.value(string) };
				record = record.add(string);
				string = String.new; 
								// or line is not empty
				if (skipEmptyLines.not or: { (record != [ "" ]) })
				{ table = table.add(record); };
				
				record = nil;
			},{
				string = string.add(c);
			})});
			
		});
				// add last string if there is one!
		if (string.notEmpty) {
			func !? { string = func.value(string) };
			record = record.add(string) 
		};
		if (record.notNil) { table = table.add(record) };
		^table
	}

	*readInterpret { arg path, skipEmptyLines=false, skipBlanks=false; 
		^this.read(path, skipEmptyLines, skipBlanks, _.interpret);
	}
	
	readInterpret { arg skipEmptyLines=false, skipBlanks=false;
		^this.read(skipEmptyLines, skipBlanks=false, _.interpret)
	}	
}

TabFileReader : FileReader { 
	classvar <delim = $\t;
}

CSVFileReader : FileReader { 
	classvar <delim = $,;
}
