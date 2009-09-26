
KeyCodeResponderGui : ObjectGui {

	guiBody { |f|
		model.dict.keysValuesDo({ |k,v|
			f.startRow;
			k.asAscii.gui(f);
			v.gui(f);
		})
	}
	writeName {}
}

KeyCodeResponderStackGui : KeyCodeResponderGui {
	guiBody { |f|
		model.stack.do({ |kdr|
			kdr.gui(f);
		})
	}
}

SimpleKDRUnitGui : KeyCodeResponderGui {

	guiBody { |f|
		CXLabel(f, KeyCodeResponder.maskToString(model.requireMask));
		(model.description  ? model.function).gui(f);
	}
}




