//sort to somewhere else later


+Collection {
	writeInputSpec { arg file, synthDef;
		this.do({ arg item; item.writeInputSpec(file, synthDef) });
	}
}