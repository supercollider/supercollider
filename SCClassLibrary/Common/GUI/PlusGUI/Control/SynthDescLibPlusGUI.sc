+ SynthDescLib {
	browse {
		var w,wleft,wright, synthView;
		var synthDescLib;
		var synthDescLibListView;
		var synthDescListView;
		var ugensListView;
		var controlsListView;
		var inputsListView;
		var outputsListView;
		var synthDescList;
		var hvBold12;
		var updateSynthDefs;
		var updateSynthDefData;
		var updateSynthWindow;
		var btn, testFn;
		var fntMono, gui;

		hvBold12 = Font.sansSerif( 12 ).boldVariant;
		fntMono	= Font.monospace( 10 );

		w = gui.window.new("SynthDef browser", Rect(128, (gui.window.screenBounds.height - 638).clip(0, 320),
			680+440, 608));
		w.view.decorator = FlowLayout(w.view.bounds,0@0,0@0 );

		wleft = CompositeView.new( w, 680@608 );
		wleft.decorator = FlowLayout(wleft.bounds);

		wright = ScrollView.new( w, 440@608 );

		wleft.decorator.shift(220);

		testFn = {
			var synth, item;
			item = this[synthDescListView.item.asSymbol];
			if (item.notNil) {
				synth = Synth(item.name);
				synth.postln;
				synth.play;
				SystemClock.sched(3, {
					if (item.hasGate)
						{ synth.release }
						{ synth.free };
				});
			};
		};

		btn = gui.button.new(wleft, 48 @ 20);
		btn.states = [["test"]];
		btn.action = testFn;

		btn = gui.button.new(wleft, 48 @ 20);
		btn.states = [["window"]];
		btn.action = {
			var item;
			item = this[synthDescListView.item.asSymbol];
			if (item.notNil) { item.makeWindow };
		};

		wleft.decorator.nextLine;
		gui.staticText.new(wleft, Rect(0,0,220,24)).string_("SynthDescLibs").font_(hvBold12);
		gui.staticText.new(wleft, Rect(0,0,220,24)).string_("SynthDefs").font_(hvBold12);
		gui.staticText.new(wleft, Rect(0,0,220,24)).string_("UGens").font_(hvBold12);
		wleft.decorator.nextLine;

		synthDescLibListView = gui.listView.new(wleft, Rect(0,0, 220, 320)).focus;
		synthDescListView = gui.listView.new(wleft, Rect(0,0, 220, 320));
		synthDescListView.beginDragAction_({arg v;
			v.items[v.value].asSymbol;
		});
		ugensListView = gui.listView.new(wleft, Rect(0,0, 220, 320));

		wleft.decorator.nextLine;
		gui.staticText.new(wleft, Rect(0,0,240,24)).string_("SynthDef Controls")
			.font_(hvBold12).align_(\center);
		gui.staticText.new(wleft, Rect(0,0,200,24)).string_("SynthDef Inputs")
			.font_(hvBold12).align_(\center);
		gui.staticText.new(wleft, Rect(0,0,200,24)).string_("SynthDef Outputs")
			.font_(hvBold12).align_(\center);
		wleft.decorator.nextLine;

		controlsListView = gui.listView.new(wleft, Rect(0,0, 240, 160));
		inputsListView = gui.listView.new(wleft, Rect(0,0, 200, 160));
		outputsListView = gui.listView.new(wleft, Rect(0,0, 200, 160));
		controlsListView.resize = 4;
		inputsListView.resize = 4;
		outputsListView.resize = 4;

		[controlsListView, inputsListView, outputsListView].do {
				|listview| listview.selectionMode = \none
		};

		controlsListView.font	= fntMono;
		inputsListView.font	= fntMono;
		outputsListView.font	= fntMono;

		wleft.decorator.nextLine;

		synthDescLibListView.items_(SynthDescLib.all.keys.asArray.sort)
			.value_(synthDescLibListView.items.indexOf(name) ? 0);
		synthDescLibListView.action = {
			synthDescListView.value = 0;
			updateSynthDefs.value;
		};

		synthDescListView.items = [];
		synthDescListView.action = {
			updateSynthDefData.value;
			updateSynthWindow.value;
		};
		synthDescListView.enterKeyAction = testFn;

		updateSynthDefs = {
			var libName;

			libName = synthDescLibListView.item;
			synthDescLib = SynthDescLib.all[libName];
			synthDescList = synthDescLib.synthDescs.values.sort {|a,b| a.name <= b.name };
			synthDescListView.items = synthDescList.collect {|desc| desc.name.asString };

			updateSynthDefData.value;
		};

		updateSynthWindow = {
			var synthDesc;
			wright.removeAll; // remove the children windows
			if ( synthDescListView.value.notNil ){
				synthDesc = synthDescList[synthDescListView.value];
			};
			if (synthDesc.notNil) {
				synthDesc.makeWindow( wright, Color.yellow(0.95) ); // create a new window
			}
		};

		updateSynthDefData = {
			var synthDesc;

			if ( synthDescListView.value.notNil ){
				synthDesc = synthDescList[synthDescListView.value];
			};

			if (synthDesc.isNil) {
				ugensListView.items = [];
				inputsListView.items = [];
				outputsListView.items = [];
				controlsListView.items = [];
			}{
				ugensListView.items = synthDesc.def.children.collect { |x, i|
					i.asString.copy.extend(7, $ ) ++ x.class.name.asString;
				};

				inputsListView.items = synthDesc.inputs.collect { |x|
					var string;
					string = x.rate.asString.copy;
					string = string.extend(9, $ ) ++ " " ++ x.startingChannel;
					string = string.extend(19, $ ) ++ " " ++ x.numberOfChannels;
				};
				outputsListView.items = synthDesc.outputs.collect { |x|
					var string;
					string = x.rate.asString.copy;
					string = string.extend(9, $ ) ++ " " ++ x.startingChannel;
					string = string.extend(19, $ ) ++ " " ++ x.numberOfChannels;
				};
				controlsListView.items = synthDesc.controls.reject {|a|
					a.name == '?'
				}.collect { |x|
					var string;
					string = if (x.name.notNil) { x.name.asString.copy; }{ "" };
					if (x.rate.notNil) {
						string = string.extend(12, $ ) ++ " " ++ x.rate;
					};

					if (x.defaultValue.notNil) {
						if (x.defaultValue.isArray) {
							string = string.extend(22, $ ) ++ " " ++ x.defaultValue.collect(_.asStringPrec(6));
						} {
							string = string.extend(22, $ ) ++ " " ++ x.defaultValue.asStringPrec(6);
						}
					};
				};
			};
		};

		updateSynthDefs.value;

		w.front;
	}
}
