

+ SCWindow {

	asFlowView { arg bounds;
		^FlowView(this,bounds)
	}
	asPageLayout { arg title,bounds;
		^PageLayout.on(this,bounds)
	}
}

+ SCCompositeView {

	asFlowView { arg bounds;
		^FlowView(this,bounds ?? {this.bounds})
	}
}

+ SCLayoutView {

	asFlowView { |bounds|
		^FlowView(this,bounds ?? {this.bounds})
	}
}

+ SCContainerView {

	asPageLayout { arg title,bounds;
		^FlowView(this,bounds ?? {this.bounds})
	}
}

