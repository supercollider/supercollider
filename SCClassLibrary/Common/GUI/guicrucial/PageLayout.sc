

PageLayout  {

	// a Window with a FlowView on it
	// it also manages onClose handlers for use by ObjectGui's MVC model,

	var <window,<view;
	var <>isClosed=false,boundsWereExplicit=false,<>onClose;

	var autoRemoves;

	*new { arg title,bounds,margin,background,scroll=true,front=true;
		^super.new.init(title,bounds,margin,background,scroll,front)
	}

	init { arg title,bounds,argMargin,background,argScroll=true,front=true;
		var w,v;
		if(bounds.notNil,{
			boundsWereExplicit = true;
			bounds = bounds.asRect
		},{
			bounds = GUI.window.screenBounds.insetAll(10,20,0,25)
		});
		window = GUI.window.new("< " ++ title.asString ++ " >",bounds, border: true, scroll: argScroll );
		window.onClose_({ this.close });
		if(background.isKindOf(Boolean),{  // bwcompat : metal=true/false
			background = background.if(nil,{Color(0.88, 0.94, 0.87, 1)})
		});

		if(background.notNil,{
			window.view.background_(background);
		});
		isClosed = false;
		view =  FlowView.new( window, window.view.bounds.insetAll(4,4,0,0)  );
		view.decorator.margin_(argMargin ?? {GUI.skin.margin});
		autoRemoves = [];
		if(front,{ window.front });
	}
	*on { arg window,bounds,margin,background;
		^super.new.initOn(window,bounds,margin,background)
	}
	initOn { arg argWindow,bounds,argMargin,background;
		window = argWindow;
		view = FlowView.new(window,bounds);
		if(argMargin.notNil,{
			view.decorator.margin_(argMargin);
		});
		autoRemoves = [];
	}
	asView { ^this.view.asView }
	asFlowView { arg bounds;
		^if(bounds.notNil,{
			FlowView(this,bounds)
		},{
			this.view
		})
	}
	bounds { ^this.view.bounds }
	add { arg view;
		this.view.add(view);
	}
	asPageLayout { arg name,bounds;
		if(isClosed,{
			^this.class.new(name,bounds)
		},{
			^this
		})
	}
	startRow {
		this.view.startRow;
	}
	indentedRemaining { ^this.view.indentedRemaining }
	*guify { arg parent,title,width,height,background;
		^parent ?? {
			this.new(title,width@height,background: background )
		}
	}

	// act like a GUI window
	checkNotClosed { ^isClosed.not }
	front {
		window.front
	}
	hide {
		window.alpha = 0.0;
	}
	show {
		window.alpha = 1.0;
	}
	close { // called when the GUI.window closes
		if(isClosed.not,{
			isClosed = true;
			autoRemoves.do({ arg updater; updater.remove(false) });
			autoRemoves = nil;
			window.close;
			onClose.value;
			NotificationCenter.notify(window,\didClose);
			window=view=nil;
		});
	}
	refresh {
		window.refresh
	}
	hr { arg color,height=8,borderStyle=1;
		this.view.hr;
	}

	focus { arg index=0;
		var first;
		first = this.window.views.at(index);
		if(first.notNil,{first.focus });
	}

	background_ { arg c; this.view.background_(c) }

	removeOnClose { arg dependant;
		autoRemoves = autoRemoves.add(dependant);
	}

	resizeToFit { arg reflow=false,center=false;
		var fs, b,wb,wbw,wbh;

		b = this.view.resizeToFit(reflow);
		if(GUI.scheme.id == \cocoa,{
			wbw = b.width + 4;
			wbh = b.height + 17;
		},{
			wbw = b.width + 11;
			wbh = b.height + 15;
		});
		window.setInnerExtent(wbw,wbh);

		if(center and: {window.respondsTo(\setTopLeftBounds)}) {
			// this should be a window method
			fs = GUI.window.screenBounds;
			wb = window.bounds;
			// bounds are inaccurate until the end of the code cycle/refresh
			wb.width = wbw;
			wb.height = wbh;
			// if height is less than 60% of full screen
			if(wbh <= (fs.height * 0.6),{
				// then move its top to be level at golden ratio
				wb.top = fs.height - (fs.height / 1.6180339887);
			},{
				wb.top = 0;
			});
			// center it horizontally
			wb.left = (fs.width - wbw) / 2;
			window.setTopLeftBounds(wb);
		}
	}
	reflowAll {
		view.reflowAll;
	}
	fullScreen {
		window.bounds = GUI.window.screenBounds.insetAll(10,20,0,25);
	}
	endFullScreen {
		window.endFullScreen
	}
	flow { arg func,bounds;
		^this.view.flow(func,bounds)
	}
	vert { arg func,bounds,spacing;
		^this.view.vert(func,bounds,spacing)
	}
	horz { arg func,bounds,spacing;
		^this.view.horz(func,bounds,spacing)
	}
	comp { arg func,bounds;
		^this.view.comp(func,bounds)
	}
	scroll { arg ... args;
		^this.view.performList(\scroll,args)
	}	
}

