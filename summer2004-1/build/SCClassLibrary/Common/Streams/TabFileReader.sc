TabFileReader {
	// a class to read tab delimited files.
	var stream;
	
	*new { arg stream;
		^super.newCopyArgs(stream)
	}
	
	read {
		var string, record, table, c;
		
		string = String.new;
		while ({
			c = stream.get;
			c.notNil
		},{
			if (c == $\t, {
				record = record.add(string);
				string = String.new;
			},{
			if (c == $\n or: { c == $\r }, {
				table = table.add(record);
				record = nil;
			},{
				string = string.add(c);
			})});
		});
		^table
	}
}

