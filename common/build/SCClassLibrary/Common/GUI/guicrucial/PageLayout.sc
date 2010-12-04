
PageLayout  {
	/* creates and manages the open/close lifecycle of a window with a top level FlowView.
		it also manages onClose handlers for use by ObjectGui's MVC model,
		intelligent resizing
		 
		this was previously called MultiPageLayout (for historical reasons)
		which is now an innacurate name.
		the MultiPageLayout class is in cruciallib pending deprecation
	*/
	var <window,<view;
	var <>isClosed=false,boundsWereExplicit=false;

	var autoRemoves;

	*new { arg title,bounds,margin,background,scroll=true,front=true;
		^super.new.init(title,bounds,margin,background,scroll,front)
	}

	init { arg title,bounds,argMargin,background,argScroll=true,front=true;
		var w,v;
		bounds = if(bounds.notNil,{
					boundsWereExplicit = true;
					bounds.asRect
				},{
					GUI.window.screenBounds.insetAll(10,20,0,25)
				});
		window = GUI.window.new("< " ++ title.asString ++ " >",
					bounds, border: true, scroll: argScroll )
			.onClose_({
				this.close; // close all windows in this layout
			});
		if(background.isKindOf(Boolean),{  // bwcompat : metal=true/false
			background = background.if(nil,{Color(0.886274509803, 0.94117647058824, 0.874509803921, 1)})
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
	asView { ^this.view }
	bounds { ^this.view.bounds }
	add { arg view;
		this.view.add(view);
	}
	asPageLayout { arg name,bounds;
		if(isClosed,{
			^this.class.new(name,bounds)
		})// else this
	}
	startRow {
		this.view.startRow;
	}
	indentedRemaining { ^this.view.indentedRemaining }
	*guify { arg parent,title,width,height,background;
		^parent ?? {
			this.new(title,width,height,background: background )
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
			window.do({ arg w;
				w.close;
			});
			window=view=nil;
			NotificationCenter.notify(this,\didClose);
		});
	}
	onClose_ { arg f; window.onClose = f; }
	refresh {
		window.refresh
	}
	hr { arg color,height=8,borderStyle=1; // html joke
		this.view.hr;
	}
	// wow, old school
	layRight { arg w,h; ^Rect(0,0,w,h) }

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
		wbw = b.width + 4;
		wbh = b.height + 17;
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

}
