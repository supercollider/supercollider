

+ QWindow {

	asPageLayout { arg title,bounds;
		^PageLayout.on(this,bounds)
	}
}


+ QView {
	
	asPageLayout { arg title,bounds;
		^FlowView(this,bounds ?? {this.bounds})
	}
}

