

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
	*antiAliasing_ { arg flag = false;
		_Font_SetAntiAliasing;
		^this.primitiveFailed
	}
	*smoothing_ { arg flag = false;
		_Font_SetSmoothing;
		^this.primitiveFailed
	}
	storeArgs { ^[name,size] }
}

