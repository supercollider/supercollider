(
//use as table
var size, win, data1, data2, view1, view2, swapData, data1Color, data2Color;
data1Color = Color.blue;
data2Color = Color.green;
size = 350 / 6;
data1 = {1.0.rand} ! size;
data2 = {1.0.rand} ! size;

win = Window("double multislider", Rect(200 , 450, 450, 150));

Button(win, Rect(0,0, 80,20)).states_([["green"],["blue"]])
			.action_{|v| swapData.(v.value)};

swapData = {|dofocusOn|
	view1.remove;
	view2.remove;
	if(dofocusOn == 0){
	view1 = MultiSliderView(win, Rect(0, 24, 350, 100))
			.background_(Color.green(alpha:0.0))
			.fillColor_(data1Color.alpha_(0.5))
			.value_(data1)
			.isFilled_(true)
			.indexThumbSize_(2)
			.gap_(4)
			.action_({|sl| data1 = sl.value;});
	view2 = MultiSliderView(win, Rect(0, 24, 350, 100))
			.background_(Color.green.alpha_(0.0))
			.fillColor_(data2Color)
			.value_(data2)
			.isFilled_(true)
			.indexThumbSize_(2)
			.gap_(4)
			.action_({|sl| data2 = sl.value;});
	}{
	view2 = MultiSliderView(win, Rect(0, 24, 350, 100))
			.background_(Color.green(alpha:0.0))
			.fillColor_(data2Color.alpha_(0.5))
			.value_(data2)
			.isFilled_(true)
			.indexThumbSize_(2)
			.gap_(4)
			.action_({|sl| data2 = sl.value;});
	view1 = MultiSliderView(win, Rect(0, 24, 350, 100))
			.background_(Color.green.alpha_(0.0))
			.fillColor_(data1Color)
			.value_(data1)
			.isFilled_(true)
			.indexThumbSize_(2)
			.gap_(4)
			.action_({|sl| data1 = sl.value;});
	}

};

swapData.value(0);
win.front;
)
