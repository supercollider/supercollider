

Font {
	classvar <>default;
	var <>name, <>size;
	
	*new { arg name, size;
		^super.newCopyArgs(name, size);
	}
	setDefault { default = this }
}

