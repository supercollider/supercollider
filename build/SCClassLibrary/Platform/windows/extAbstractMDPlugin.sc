
// windows needs a different way to clear synthdef metadata

+ AbstractMDPlugin {
	*clearMetadata { |path|
		path = path.splitext[0].do({ |chr, i| if(chr == $/) { path[i] = $\\.asAscii } });
		"del %%.*meta%".format(34.asAscii, path, 34.asAscii).systemCmd;
	}
}
