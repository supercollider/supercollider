TabFileReader {
	// a class to read tab delimited files.
	var stream;
	
	*new { arg stream;
		^super.newCopyArgs(stream)
	}
	
	*read { arg path, skipEmptyLines=false; 
		var f, table; 
		f = File(path, "r"); 
		if (f.isOpen.not) { warn("TabFileReader: file" + path + "not found.") ^nil };
		table = this.new(f).read(skipEmptyLines);
		f.close;
		^table;
	}
	read { arg skipEmptyLines=false; 
		var string, record, table, c;
		
		string = String.new;
		while ({
			c = stream.getChar;
			c.notNil
		},{
			if (c == $\t, {
				record = record.add(string);
				string = String.new;
			},{
			if (c == $\n or: { c == $\r }, {
				record = record.add(string);
				string = String.new; 
								// or line is not empty
				if (skipEmptyLines.not or: { (record != [ "" ]) })
				{ 	table = table.add(record); };
				
				record = nil;
			},{
				string = string.add(c);
			})});
			
		});
		if (string.notEmpty) { record = record.add(string); };
		if (record.notNil) { table = table.add(record); };
		^table
	}

	*readInterpret { arg path, skipEmptyLines=false; 
		var f, table; 
		f = File(path, "r"); 
		if (f.isOpen.not) { warn("TabFileReader: file" + path + "not found.") ^nil };
		table = this.new(f).readInterpret(skipEmptyLines);
		f.close;
		^table;
	}
	
	readInterpret { arg skipEmptyLines=false; 
		var string, record, table, c;
		
		string = String.new;
		while ({
			c = stream.getChar;
			c.notNil
		},{
			if (c == $\t, {
				record = record.add(string.interpret);
				string = String.new;
			},{
			if (c == $\n or: { c == $\r }, {
				record = record.add(string.interpret);
				string = String.new; 
								// or line is not empty
				if (skipEmptyLines.not or: { (record != [ "" ]) })
				{ 	table = table.add(record); };
				
				record = nil;
			},{
				string = string.add(c);
			})});
			
		});
		if (string.notEmpty) { record = record.add(string.interpret); };
		if (record.notNil) { table = table.add(record); };
		^table
	}	
}
