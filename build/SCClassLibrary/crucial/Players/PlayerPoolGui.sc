
PlayerPoolGui : AbstractPlayerGui {

	var triggers,guis,rect,selectedBox;

	guiBody { arg layout; // has a select button
		var minWidth,prev=0,indicators,wrapEvery;

		ActionButton(layout,"release",{ this.releaseAll });
		if(model.list.notEmpty,{
			minWidth = model.list.maxItem({ arg sf; sf.name.asString.size }).name.asString.size * 9;
			wrapEvery = (layout.innerBounds.width / (minWidth + 100)).asInteger;

			triggers =
			model.list.collect({arg sf,i;
				var ind;
				if(i % wrapEvery == 0,{ layout.startRow });
				ind = GUI.button.new(layout,20@16);
				ind.action = {
					this.unselect(model.selected);
					model.select(i);
					this.select(i);
				};
				ind.states = [[" ",Color.green,Color.grey],[" ",Color.grey,Color.green]];
				Tile(sf,layout,minWidth: minWidth);
				ind
			});
		});
		selectedBox =  GUI.compositeView.new(layout,Rect(0,0,layout.innerBounds.width,500));

		rect = selectedBox.bounds;
		guis = Array.newClear(model.list.size);
		//this.select(model.selected);
	}

	releaseAll {
		triggers.at(model.selected).setProperty(\value, 0);
		model.releaseVoice;
	}
	unselect { arg i;
		var g;
		g = guis.at(i);
		if(g.notNil,{
			g.visible = false;
		});
		triggers.at(i).setProperty(\value, 0); // dim
	}
	select { arg i;
		var gui;
		triggers.at(i).setProperty(\value, 1);
		if((gui = guis.at(i)).isNil,{
			guis.put(i,model.list.at(i).gui(selectedBox,rect));
		},{
			gui.visible = true;
		});
	}

}
