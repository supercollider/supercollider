

HasSubjectGui : AbstractPlayerGui {
	//smallGui could not gui the subject
	guiBody { arg layout;
		CXLabel(layout,"on:");
		layout.indent(1);
		model.subject.gui(layout);
		layout.indent(-1);
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
		//if(changer !== this,{
			num.value = model.amp;
		//})
	}
}

KrGui : HasSubjectGui {

//	guiBody { arg layout;
//		CXLabel(layout,"lag:");
//		model.lag.smallGui(layout);
//		super.guiBody(layout);
//	}
}


StreamKrGui : KrGui {

	guiBody { arg layout;
		CXLabel(layout,"durs:");
		model.durations.smallGui(layout);
		super.guiBody(layout);
	}

}


// StreamKrDur can simply inherit
