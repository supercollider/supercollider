+ HistoryGui {
	postInlined { |index|
		thisMethod.deprecated(
			HistoryGui.methods.detect { |m| m.name == \showLineAt }
		);
		this.showLineAt(index);
	}
}
