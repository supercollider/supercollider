

+ Window {

	asPageLayout { arg title, bounds;
		^PageLayout.on(this, bounds)
	}
}


+ View {

	asPageLayout { arg title, bounds;
		^FlowView(this, bounds ?? {this.bounds})
	}
}
