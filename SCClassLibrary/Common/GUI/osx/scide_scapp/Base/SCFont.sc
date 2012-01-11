SCFont {
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

	boldVariant {
		^if( name.endsWith( "-Bold" ), this, { this.class.new( name ++ "-Bold", size )});
	}

	*defaultSansFace {
		^"Helvetica";
	}

	*defaultSerifFace {
		^"Times";
	}

	*defaultMonoFace {
		^"Monaco";
	}

	*monospace {|size|
		^this.new(this.defaultMonoFace, size)
	}

	*serif {|size|
		^this.new(this.defaultSerifFace, size)
	}

	*sansSerif {|size|
		^this.new(this.defaultSansFace, size)
	}

	// support Qt GUI in combination with CocoaDocument
	asSCFont {
		^this
	}
}
