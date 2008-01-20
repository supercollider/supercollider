+ EZSlider { 
	setVal { arg val; 
		numberView.value = value = controlSpec.constrain(val);
		sliderView.value = controlSpec.unmap(value);
	}
}