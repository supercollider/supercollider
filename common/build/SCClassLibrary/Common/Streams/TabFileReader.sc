FileReader : Stream {
	// a class to read text files automatically
	classvar <delim = $ ;	// space separated by default
	var <stream, skipEmptyLines=false, skipBlanks=false, <delimiter;

	*new { | pathOrFile, skipEmptyLines=false, skipBlanks=false,  delimiter |
		var stream;
		if (pathOrFile.isKindOf(File) ) { stream = pathOrFile }  { stream =  File(pathOrFile, "r") };
		if (stream.isOpen.not) { warn(this.name ++ ": file" + pathOrFile + "not found.") ^nil };
		^super.newCopyArgs(stream, skipEmptyLines, skipBlanks,  delimiter ? this.delim)
	}

	reset { stream.reset }

	close { stream.close }

	next {
		var c, record, string = String.new;
		while {
			c = stream.getChar;
			c.notNil
		} {
			if (c == delimiter) {
				if (skipBlanks.not or: { string.size > 0 }) {
					record = record.add(string);
					string = String.new;
				}
			} {
				if (c == $\n or: { c == $\r }) {
					record = record.add(string);
					string = String.new;
					if (skipEmptyLines.not or: { (record != [ "" ]) }) {
						^record
					};
					record = nil;
				}{
					string = string.add(c);
				}
			}
		};
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
