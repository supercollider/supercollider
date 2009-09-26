FileReader : Stream {
	// a class to read text files automatically
	classvar <delim = $ ;	// space separated by default
	var <stream, skipEmptyLines=false, skipBlanks=false, <delimiter;

	*new { | pathOrFile, skipEmptyLines=false, skipBlanks=false,  delimiter |
		var stream;
		if (pathOrFile.isKindOf(File) ) { stream = pathOrFile }  { stream =  File(pathOrFile, "r") };
		if (stream.isOpen.not) { warn("FileReader: file" + pathOrFile + "not found.") ^nil };
		^super.newCopyArgs(stream, skipEmptyLines, skipBlanks,  delimiter ? this.delim)
	}

	reset { stream.reset }

	close { stream.close }

	next {
		var c, record, string = String.new;
		while ({
			c = stream.getChar;
			c.notNil
		},{
			if (c == delimiter) {
				if (skipBlanks.not or: { string.size > 0 }) {
					record = record.add(string);
					string = String.new;
				}
			} {
				if (c == $\n or: { c == $\r }) {
					record = record.add(string);
					string = String.new;
					if (skipEmptyLines.not or: { (record != [ "" ]) })
					{^record };
					record = nil;
				}{
					string = string.add(c);
				}
			}

		});
		if (string.notEmpty) { ^record.add(string) };
		^record;
	}

	read { ^this.all }

	*read { | path, skipEmptyLines=false, skipBlanks=false, func, delimiter, startRow = 0, skipSize = 0 |
		var fr, table;
		fr = this.new(path, skipEmptyLines, skipBlanks,  delimiter) ?? { ^nil };

		if (func.notNil) {
			table = fr.subSample(startRow, skipSize).collect(_.collect(func)).all;
		} {
			table = fr.subSample(startRow, skipSize).all;
		};
		fr.close;
		^table
	}

	*readInterpret { | path, skipEmptyLines=false, skipBlanks=false, delimiter, startRow = 0, skipSize = 0  |
		^this.read(path, skipEmptyLines, skipBlanks, _.interpret, delimiter, startRow, skipSize )	}

}

TabFileReader : FileReader {
	classvar <delim = $\t;
}

CSVFileReader : FileReader {
	classvar <delim = $,;
}

SemiColonFileReader : FileReader {
	classvar <delim = $;;
}
//FileReader {
//	// a class to read text files automatically
//	classvar <delim = $ ;	// space separated by default
//	var stream;
//
//	*new { arg stream;
//		^super.newCopyArgs(stream)
//	}
//
//	*read { arg path, skipEmptyLines=false, skipBlanks=false, func, delimiter, startRow=0, subsample=1;
//		var f, table;
//		f = File(path, "r");
//		if (f.isOpen.not) { warn("FileReader: file" + path + "not found.") ^nil };
//		table = this.new(f).read(skipEmptyLines, skipBlanks, func, (delimiter), startRow, subsample);
//		f.close;
//		^table;
//	}
//	read { arg skipEmptyLines=false, skipBlanks=false, func, delimiter, startRow=0, subsample=1;
//		var string, record, table, c, subsampctr, subsampfunc;
//
//		delimiter = delimiter ? this.class.delim;
//		string = String.new;
//		subsampctr = 0; // Used for subsampling
//
//		// subsample can be a function (e.g. {0.5.coin} for randomised subsampling) or an integer for uniform subsampling.
//		// We convert subsample into a function that the read loop can use
//		subsampfunc = if(subsample == 1){
//			true	// Optimise for common case, just give "true" rather than a function!
//		}{
//			if(subsample.isFunction){subsample}{
//				// For integer values we build a simple counter - this function:
//				{if(subsampctr % subsample == 0){
//					subsampctr = 1;
//					true
//				}{
//					subsampctr = subsampctr + 1;
//					false
//				}}
//			}
//		};
//
//		// Now do the actual file reading:
//		while ({
//			c = stream.getChar;
//			c.notNil
//		},{
//			if (c == delimiter, {
//				if (skipBlanks.not or: { string.size > 0 }) {
//					func !? { string = func.value(string) };
//					record = record.add(string);
//					string = String.new;
//				}
//			},{
//			if (c == $\n or: { c == $\r }, {
//				func !? { string = func.value(string) };
//				record = record.add(string);
//				string = String.new;
//								// or line is not empty
//				if (skipEmptyLines.not or: { (record != [ "" ]) })
//				{
//					// We have a completed record. Are we going to add it?
//					if(startRow > 0){
//						startRow = startRow - 1; // startRow used as a countdown
//					}{
//						if(subsampfunc.value){
//							table = table.add(record);
//						}
//					}
//				};
//
//				record = nil;
//			},{
//				string = string.add(c);
//			})});
//
//		});
//				// add last string if there is one!
//		if (string.notEmpty) {
//			func !? { string = func.value(string) };
//			if(startRow > 0 and:{subsampfunc.value}){
//				record = record.add(string)
//			}
//		};
//		if (record.notNil) { table = table.add(record) };
//		^table
//	}
//
//	*readInterpret { arg path, skipEmptyLines=false, skipBlanks=false;
//		^this.read(path, skipEmptyLines, skipBlanks, _.interpret);
//	}
//
//	readInterpret { arg skipEmptyLines=false, skipBlanks=false;
//		^this.read(skipEmptyLines, skipBlanks=false, _.interpret)
//	}
//}
//
//TabFileReader : FileReader {
//	classvar <delim = $\t;
//}
//
//CSVFileReader : FileReader {
//	classvar <delim = $,;
//}
//
//SemiColonFileReader : FileReader {
//	classvar <delim = $;;
//}