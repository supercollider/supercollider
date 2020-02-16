DebugTable {
	var <functionDef, <debugTable;
	var <size;

	*new {
		|def, table|
		^super.newCopyArgs(def, table, table.size / 2)
	}

	at {
		|i|
		var fnSymbol = functionDef.respondsTo(\filenameSymbol).if({ functionDef.filenameSymbol }, 'anonymous');
		^DebugTableLine(fnSymbol, debugTable[i * 2], debugTable[i * 2 + 1], functionDef.sourceCode)
	}

	do {
		|func|
		size.do {
			|i|
			func.value(this.at(i), i);
		}
	}
}

DebugTableLine {
	var <filenameSymbol, <line, <character, <rawSource;

	*new {
		|filenameSymbol, line, character, source|
		^super.newCopyArgs(filenameSymbol, line, character, source)
	}

	sourceLine {
		|preContext=0, postContext=0|
		if (File.exists(filenameSymbol.asString).not) {
			if (rawSource.notNil) {
				var a, b;
				a = line - preContext - 1;
				b = line + postContext;
				^rawSource.split($\n)[a..b]
			} {
				Exception("Source file not found: %".format(filenameSymbol)).throw
			}
		} {
			var f = File(filenameSymbol.asString.standardizePath, "r");
			var source = List();
			(line - preContext - 1).do {
				f.getLine(2048)
			};
			(preContext + postContext + 1).do {
				source.add(f.getLine(2048));
			};
			^source
		}
	}

	show {
		if (filenameSymbol != \anonymous) {
			if (File.exists(filenameSymbol.asString).not) {
				Exception("Source file not found: %".format(filenameSymbol)).throw
			} {
				fork {
					var doc, docSize, currentLine = 1, currentChar = 0;
					doc = Document.open(filenameSymbol.asString);
					docSize = doc.string.size;
					doc.front;

					while {currentChar < docSize} {
						if (doc.string[currentChar] == $\n) {
							currentLine = currentLine + 1;
						};

						currentChar = currentChar + 1;

						if (currentLine >= line) {
							doc.selectRange(currentChar, character);
							currentChar = docSize;
						}
					}
				}
			}
		}
	}

	printOn {
		|stream|
		stream << "DebugTableLine[% %:%]".format(filenameSymbol, line, character)
	}
}

+FunctionDef {
	debugTable {
		// ^DebugTable(this, debugTable)
	}
}


