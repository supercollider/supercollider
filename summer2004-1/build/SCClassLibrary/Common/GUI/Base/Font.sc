

Font {
	classvar <>default;
	var <>name, <>size;
	
	*new { arg name, size;
		^super.newCopyArgs(name, size);
	}
	setDefault { default = this }
	
	*availableFonts {
		// returns an Array of font names.
		_Font_AvailableFonts
		^this.primitiveFailed
	}
	*setAntiAliasing { arg boo = false;
		_Font_SetAntiAliasing;
		^this.primitiveFailed
	}
	storeArgs { ^[name,size] }
}

