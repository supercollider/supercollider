+ SCWindow {
	asPageLayout { arg title,bounds;
		^MultiPageLayout.on(this,bounds)
	}
	asFlowView { arg bounds;
		^FlowView(this,bounds)
	}
}
+ SCContainerView {
	asPageLayout { arg title,bounds;
		// though it won't go multi page
		// FlowView better ?
		^FlowView(this,bounds ?? {this.bounds})
		//^MultiPageLayout.on(this,bounds)
	}
}
//+ SCViewHolder {
//	asPageLayout { arg title,bounds;
//		//this.insp("on...");
//		^FlowView(this,bounds)
//		//^MultiPageLayout.on(this,bounds)
//	}
//}
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

