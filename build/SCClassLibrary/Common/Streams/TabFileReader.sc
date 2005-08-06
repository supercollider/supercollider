TabFileReader {
	// a class to read tab delimited files.
	var stream;
	
	*new { arg stream;
		^super.newCopyArgs(stream)
	}
	
	*read { arg path, skipEmptyLines=false, func; 
		var f, table; 
		f = File(path, "r"); 
		if (f.isOpen.not) { warn("TabFileReader: file" + path + "not found.") ^nil };
		table = this.new(f).read(skipEmptyLines, func);
		f.close;
		^table;
	}
	read { arg skipEmptyLines=false, func; 
		var string, record, table, c;
		string = String.new;
		while ({
			c = stream.getChar;
			c.notNil
		},{
			if (c == $\t, {
				func !? { string = func.value(string) };
				record = record.add(string);
				string = String.new;
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
		
		if (string.notEmpty) {
			func !? { string = func.value(string) };
			record = record.add(string) 
		};
		if (record.notNil) { table = table.add(record) };
		^table
	}

	*readInterpret { arg path, skipEmptyLines=false; 
		^this.read(path, skipEmptyLines, _.interpret);
	}
	
	readInterpret { arg skipEmptyLines=false;
		^this.read(skipEmptyLines, _.interpret)
	}	
}
