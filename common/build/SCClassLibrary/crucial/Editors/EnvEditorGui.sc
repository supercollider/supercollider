
EnvEditorGui : ObjectGui {


	var ev,timeScale=1.0,levelScale = 1.0;

	writeName {}
	guiBody { arg layout;

		var curves,levels,times,temp=0.0;

		ev = GUI.envelopeView.new(layout,layout.layRight(100,40))
			.thumbSize_(5);
		ev.drawLines = true;
		ev.selectionColor = Color.red;
		ev.drawRects = true;

		times = [0.0];
		model.env.times.do({ arg delta;
			times = times.add(temp = temp + delta);
		});
		levels = model.env.levels;

		timeScale = times.maxItem * 1.5;
		levelScale = levels.maxItem * 1.5;
		ev.value_([times / timeScale,levels / levelScale]);//("initial setting"));
		ev.action = {
			var index,levels,times,deltas,sum=0;
			index = ev.index;
			times = ev.value.at(0);
			levels = ev.value.at(1);
			times = times.collect({ arg t,i;
						if(i == 0,{
							0
						},{
							t.clip(times.at(i - 1),times.at(i + 1) ? 1);
						});
					});
			(times.size - 1).do({ arg i;
							model.putTime(i, times.at(i + 1) - times.at(i) * timeScale );
						});
			model.putLevel(index,levels.at(index) * levelScale);
			//visual clip
			//ev.value_([ times,levels]);
			ev.refresh;
		};
		// test button
		/*ActionButton(layout,"test",{
			model.env.test
		});*/
		ActionButton(layout,"#",{
			model.env.asCompileString.postln;
		});

		CXLabel(layout,"curve:");
		// curves
		curves = model.env.curves;
		if(curves.isKindOf(SimpleNumber),{
			NumberEditor(curves,[-10.0,10.0]).action_({ arg val; model.setCurve(val);  })
				.smallGui(layout);
		},{
			if(curves.isKindOf(SequenceableCollection),{
					curves.do({ arg l,i;
						NumberEditor(l,[-10.0,10.0])
							.action_({ arg val; model.putCurve(i,val) })
							.smallGui(layout);
					});
			},{
				curves.gui(layout); //symbol \linear,\sine etc.
			});
		});
		// loop node
		//CXLabel(layout,"loop");
		//CXLabel(layout,model.env.loopNode);

		//release node
		CXLabel(layout,"releaseNode:");
		CXLabel(layout,model.env.releaseNode);

		// the envelope itself
		//model.env.gui(layout);
	}
}
