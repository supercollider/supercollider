/*
// views and EZGuis may trigger actions more often that desired:
// when the spec quantizes the range of the slider, the action can fire
// without the value having changed. valueActionIfChanged_ handles this case.
(
g = EZSlider(label:" test ", controlSpec:[0, 5, \lin, 1].asSpec);
g.action_({ |ez| "ezsl.value: % sliderpos: %\n".postf(ez.value, ez.sliderView.value); });
)
(
g = EZNumber(label:" test ", controlSpec:[0, 5, \lin, 1].asSpec);
g.action_({ |ez| "ezsl.value: % sliderpos: %\n".postf(ez.value, ez.numberView.value); });
);
g = EZText(label:" test ", action: { |ez| "new value: %\n".postf(ez.value); });

*/

+ View {
	valueActionIfChanged_ { arg val;
		var hasChanged = this.value != val;
		this.value_(val);
		if (hasChanged) { this.doAction }
	}
}

+ EZGui {
	valueActionIfChanged_ { arg val;
		var hasChanged = value != val;
		this.value_(val);
		if (hasChanged) { this.doAction }
	}
}