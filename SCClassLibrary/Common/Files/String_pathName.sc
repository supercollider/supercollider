// All methods from PathName moved to String class,
// so PathName methods only redirect here.

+ String {

	// new final conversion:
	asPathString { ^this.standardizePath }
	fullPath { ^this }

	asPath { ^Path(this) }

	// keep these for speed: pathMatch returns strings,
	// and it is faster todistinguish files and folders here.
	isFolder { ^this.last.isPathSeparator }

	isFile {
		var path = this.pathMatch;
		^if(path.notEmpty, {
			path.at(0).last.isPathSeparator.not
		}, { false })
	}
}
