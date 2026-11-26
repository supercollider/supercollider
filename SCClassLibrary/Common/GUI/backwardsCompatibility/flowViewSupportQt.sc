

+ Window {

	asPageLayout { |title, bounds|
		^PageLayout.on(this, bounds)
	}
}


+ View {

	asPageLayout { |title, bounds|
		^FlowView(this, bounds ?? { this.bounds })
	}
}
