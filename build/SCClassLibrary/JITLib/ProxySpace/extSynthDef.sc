//sort to somewhere else later
+Node {
	prIsPlaying { arg flag; isPlaying = true }

}


+Collection {
	writeInputSpec { arg file, synthDef;
		this.do({ arg item; item.writeInputSpec(file, synthDef) });
	}
}