// downloaded quarks
LocalQuarks
{
	var <path;

	*new { | path |
		^super.newCopyArgs(path)
	}

	quarks {
		var pipe = Pipe("find" + path + "-type f -name QUARK", "r");
		var line, quarks = [], relativePath;
		while { line = pipe.getLine; line.notNil } {
			relativePath = line.copyToEnd(path.size);
			quarks = quarks.add(Quark.fromFile(line).path_( relativePath ) );
		}
		^quarks
	}
	findQuark { arg name, version;
		var matches;
		matches = this.quarks.collect({ |q| q.name == name });
		if(version.notNil,{
			matches = matches.select({ |q| q.version >= version });
		});
		^matches.sort({ |a,b| a.version > b.version }).first
	}

}

