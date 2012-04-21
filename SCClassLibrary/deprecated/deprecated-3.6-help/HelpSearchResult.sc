HelpSearchResult {
	var <>docname, <>path, <>goodness, <>context, <>catpath;
	*new{|docname, path, goodness, context, catpath|
		^this.newCopyArgs(docname, path, goodness, context, catpath);
	}

	asString {
		^ "HelpSearchResult(%, %, %, %)".format(docname, path.basename, goodness, this.contextTrimmed)
	}
	// used for sorting:
	<= { |that|
		^ this.goodness >= that.goodness
	}

	contextTrimmed {
		^context.tr($\n, $ ).tr($\t, $ )
	}

	drawRow { |parent, bounds, action|
		// SCButton
		Button.new(parent, bounds.copy.setExtent(bounds.width * 0.3, bounds.height).insetBy(5, 5))
				.states_([[docname]]).action_(action ? { path.openHTMLFile });

		StaticText.new(parent, bounds.copy.setExtent(bounds.width * 0.7, bounds.height)
										.moveBy(bounds.width * 0.3, 0)
										.insetBy(5, 5))
				.string_(this.contextTrimmed);

	}
}

