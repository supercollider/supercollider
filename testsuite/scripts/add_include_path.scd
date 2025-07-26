//add unit tests to the include paths
(
var path = thisProcess.argv[0];

path ?? {path = "".resolveRelative.dirname +/+ "classlibrary"};

if(File.exists(path)) {
	postf("\nAdding include path %\n\n", path);
	LanguageConfig.addIncludePath(path);
	LanguageConfig.store;
} {
	format("Specified path doesn't exist, not adding: %", path).warn;
};

0.exit;
)
