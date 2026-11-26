Font {
	classvar <defaultSansFace, <defaultSerifFace, <defaultMonoFace;

	var <>name, <size, <>bold, <>italic, <hasPointSize = false;
	/* ----------------- class -------------------*/


	*initClass {
		if(Platform.hasQt.not) { ^nil };	// skip init on Qt-less builds

		try {
			defaultSansFace = this.prDefaultFamilyForStyle(0);
			defaultSerifFace = this.prDefaultFamilyForStyle(1);
			defaultMonoFace = this.prDefaultFamilyForStyle(2)
		} {
			"WARNING: Qt font initialisation failed".postln
		}
	}

	*new { |name, size, bold = false, italic = false, usePointSize = false|
		^super.newCopyArgs(name, size, bold, italic, usePointSize)
	}

	*availableFonts {
		_Qt_AvailableFonts
		^this.primitiveFailed
	}

	*antiAliasing_ { |flag|
	}

	*smoothing_ { |flag|
	}

	*default {
		^Font()
	}

	*default_ { |font|
		this.setDefault(font)
	}

	*setDefault { |font, class|
		this.prSetDefault(font, class !? { class.qtClass })
	}

	*monospace { |size, bold = false, italic = false, usePointSize = false|
		^this.new(this.defaultMonoFace, size, bold, italic, usePointSize)
	}

	*serif { |size, bold = false, italic = false, usePointSize = false|
		^this.new(this.defaultSerifFace, size, bold, italic, usePointSize)
	}

	*sansSerif { |size, bold = false, italic = false, usePointSize = false|
		^this.new(this.defaultSansFace, size, bold, italic, usePointSize)
	}

	/* ------------------instance------------------*/

	setDefault { |class|
		Font.setDefault(this, class)
	}

	boldVariant {
		^this.class.new(name, size, true, italic)
	}

	size_ { |pixelSize| this.pixelSize_(pixelSize) }

	pixelSize_ { |pixelSize|
		size = pixelSize;
		hasPointSize = false
	}

	pointSize_ { |pointSize|
		size = pointSize;
		hasPointSize = true
	}

	pixelSize { ^(if(hasPointSize) { nil } { size }) }

	pointSize { ^(if(hasPointSize) { size } { nil }) }

	/* ------------------ private -----------------*/

	*prSetDefault { |font, className|
		_QFont_SetDefaultFont
		^this.primitiveFailed
	}

	*prDefaultFamilyForStyle { |style = -1|
		_QFont_DefaultFamilyForStyle
		^this.primitiveFailed
	}

	printOn { |aStream|
		this.storeOn(aStream)
	}

	storeArgs { ^[name, size] }
}
