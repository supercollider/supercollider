

HasSubjectGui : AbstractPlayerGui {
	//smallGui could not gui the subject
	var subjg;
	guiBody { arg layout;
		layout.startRow;
		subjg = model.subject.gui(layout);
	}
}


PlayerAmpGui : HasSubjectGui {
	var num;

	guiBody {  arg layout;
		layout = this.guify(layout);
		this.smallGui(layout);
		model.subject.gui(layout);
	}

	smallGui { arg layout;
		var l;
		l=this.guify(layout);
		num = NumberEditor(1.0,[0,2.0]);
		num.action_({ arg val; model.amp_(val) });
		num.gui(l);
		if(layout.isNil,{ l.front });
	}
	update { arg changed,changer;
		num.value_(model.amp).changed;
	}
}

KrGui : HasSubjectGui {

//	guiBody { arg layout;
//		CXLabel(layout,"lag:");
//		model.lag.smallGui(layout);
//		super.guiBody(layout);
//	}
}


StreamKrDurGui : KrGui {

	guiBody { arg layout;
		CXLabel(layout,"durs:");
		model.durations.smallGui(layout);
		super.guiBody(layout);
	}

}


// StreamKrDur can simply inherit



