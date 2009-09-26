
MultiPageLayout  {

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
	*guify { arg lay,title,width,height,background;
		^lay ?? {
			this.new(title,width,height,background: background )
		}
	}

	// act like a GUIWindow
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
	close { // called when the GUIWindow closes
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

Sheet {
	*new { arg buildDialog,name="",bounds;
		var layout;
		layout = MultiPageLayout(name,bounds,front:false);
		buildDialog.value(layout);
		layout.resizeToFit;
		layout.front;
		^layout
	}
}

ModalDialog { // hit ok or cancel

	*new { arg buildDialog,okFunc,name="?",cancelFunc;
		var globalKeyDownFunc;
		globalKeyDownFunc = SCView.globalKeyDownAction;
		SCView.globalKeyDownAction = nil;

		Sheet({ arg layout;
			var returnObjects;

			returnObjects=buildDialog.value(layout);

			layout.startRow;
			ActionButton(layout,"OK",{
				okFunc.value(returnObjects);
				layout.close;
			});

			ActionButton(layout,"Cancel",{
				cancelFunc.value(returnObjects);
				layout.close;
			});

		},name).onClose_({ SCView.globalKeyDownAction = globalKeyDownFunc; });
	}

}




// for old usage
// to be deprec in favor of MultiPageLayout or FlowView
PageLayout  {

	// should get these from Cocoa
	classvar <>screenWidth = 1100, <>screenHeight = 700; // tibook

	classvar <>bgcolor ,<>focuscolor,<>hrcolor;

	var windows, <>vspacer=3,<>hspacer=5, minWidth=0, minHeight=0, currx, curry;
	var <margin; // max area in window we may occupy
	var <winMaxBounds,tabs=0;
	var onBoundsExceeded='newWindow',metal;
	var <>isClosed=false;

	var autoRemoves;

	*new { arg title,width,height ,posx,posy,hspacer,vspacer,metal=false;
		^super.new.init(title,width ? screenWidth,height ? screenHeight,
							posx ? 20,posy ? 20,hspacer ? 5,vspacer ? 3,
							metal)
	}

	// see also .within
	*newWithin { arg w,rect;
		^super.new.initWithin(w,rect);
	}

	//asPageLayout
	*guify { arg lay,title,width,height,metal=false;
		if(lay.notNil,{// and: {lay.checkNotClosed},{
			// if its a GUIWindow we return a flow layout on it
			if(lay.isKindOf(SCWindow),{
				^this.onWindow(lay)
			},{
				^lay
			});
		},{
			^this.new(title,width,height,metal: metal )
		})
	}

	init { arg title,width,height,posx,posy,arghspacer,argvspacer,argmetal;
		var w,gwminWidth,gwminHeight;
		hspacer = arghspacer;
		vspacer = argvspacer;
		windows=windows.add
		(
			w=GUI.window.new("< " ++ title.asString ++ " >",
				Rect.new( posx, posy, width, height ) )
			.onClose_({
						this.close; // close all windows in this layout
					})
		);
		metal = argmetal;
		if(metal.not,{
			w.view.background_(bgcolor);
		});

		isClosed = false;

		margin=Rect(0 + hspacer,0 + vspacer,
				width - (hspacer * 2),height - (vspacer * 2));
		currx=margin.left;
		curry=margin.top;

		autoRemoves = IdentitySet.new;
	}

	window { ^windows.last }
	view { ^windows.last.view }
	asView { ^this.window.view }
	//asView {}
	add { arg view;
		var b;
		b = view.bounds;
		view.bounds = this.layRight(b.width,b.height);
		this.view.add(view);
	}
	asPageLayout { arg name,width,height,x,y;
		if(isClosed,{
			^this.class.new(name,width,height,x,y)
		},{
			^this
		})
	}
	startRow { // move to new line if not already there
		if(currx != margin.left,{
			currx=margin.left;
			curry=curry+minHeight+vspacer;
			minHeight=0;
			tabs.do({
				this.layRight(10,10);
			});
		});
	}

	layRight { arg x,y,argSpacer; //space requirement
		var rect;

		// wrap if exceeds right side
		if(x+currx > margin.right,{this.startRow});
		if(y+curry > margin.bottom,{
			this.perform(onBoundsExceeded)
		});

		if((x+currx)>minWidth,{minWidth=x});
		if(y>minHeight,{minHeight=y});

		rect=Rect.new(currx,curry,x,y);
		currx=currx+x+(argSpacer?hspacer);
		^rect
	}

	layDown { arg x, y, sp;
		var rect;
		if(x>minWidth,{minWidth=x});
		if(y>minHeight,{minHeight=y});
		rect=Rect.new(currx,curry,x,y);
		curry=curry + y + (sp ? vspacer);
		^rect
	}


	// act like a GUIWindow
	checkNotClosed { ^isClosed.not }
	front {
		//windows.do({ arg w; w.unhide });
		windows.reverseDo({arg w; w.front });
		//windows.first.front;
	}
	hide {
		// for now
		windows.do({ arg w; w.alpha = 0.0; })
		//windows.do({ arg w; w.hide })
	}
	show {
		//windows.do({ arg w; w.show })
		windows.do({ arg w; w.alpha = 1.0;  })
	}
	close { // called when the GUIWindow closes
		if(isClosed.not,{
			isClosed = true;
			if(windows.notNil,{
				windows.do({ arg w;
					w.close;
				});
				windows=nil;
			});
			autoRemoves.do({ arg updater; updater.remove(false) });
			NotificationCenter.notify(this,\didClose);
		});
	}

	hr { arg color,height=8,borderStyle=1; // html joke
		var r;
		this.startRow;
		r=this.layRight(this.margin.width - (2 * hspacer), height,0);
		GUI.staticText.new(this.window,r).string_("").background_(color ? hrcolor).resize_(2)
	}
	tab {
		this.layRight(10,10);
	}
	indent { arg by = 1;
		tabs = max(0,tabs + by);
	}
	width {
		^margin.width - (tabs * 10)
	}

	focus { arg index=0;
		var first;
		first = this.window.views.at(index);
		if(first.notNil,{first.focus });
	}

	background_ { arg c; this.view.background_(c) }

	//TODO remove
	//backColor { ^this.view.background }
	//backColor_ { arg b; this.view.background_(b) }

	removeOnClose { arg dependant;
		autoRemoves = autoRemoves.add(dependant);
	}


	// create a layout within the current layout
	within { arg x,y,func;
		var l,r;
		r=this.layRight(x,y);
		l=this.class.newWithin(this.window,r);
		func.value(l);
	}


	initWithin { arg w,rect;
		windows=windows.add(w);
		margin=rect;
		currx=margin.left;
		curry=margin.top;
		autoRemoves = List.new;
	}

	resizeToFit {
		// cascade all open windows and shrink them to contents
		// margin is still set large
		windows.reverse.do({ arg w;
			var b;
			if(w.view.children.notNil,{
				b = Rect.newSides(w.view.bounds.left, w.view.bounds.top,
					 w.view.children.maxValue({ arg v; v.bounds.right })
					 		+ hspacer + hspacer ,
					 w.view.children.maxValue({ arg v; v.bounds.bottom })
					 		+ vspacer + vspacer + 45);

				b.left = w.bounds.left; // same left as it was
				b.top = screenHeight - b.top; //flip to top

				w.bounds_(b);
				margin = b.insetAll(hspacer,vspacer,hspacer,vspacer);
				// warning: subsequent windows would have the same margin
				//w.front;
			})
		})
	}

	warnError {
		"PageLayout within a PageLayout exceeded bounds !!".warn;
	}

	newWindow {
		var ow;
		ow = this.window;
		this.init("..." ++ ow.name,
				margin.right + hspacer,
				margin.bottom + vspacer,
				(ow.bounds.left + 100).wrap(0,900),
				(ow.bounds.top+10).wrap(0,300),
				hspacer,
				vspacer,
				metal);
		//this.window.view.background_(ow.view.background);
		tabs.do({
			this.layRight(10,10);
		});
	}

	// make PageLayout gui objects dropable
	*onWindow { arg w;
		^this.newWithin(w,w.bounds.insetAll(5,5,5,5)).findPosition;
	}
	findPosition {
		var w;
		var bounds,lowest,band,overlaps;
		w = this.window;
		bounds = w.children.collect({ arg v; v.bounds });
		if(bounds.notEmpty,{
			lowest = bounds.maxItem({ arg b; b.bottom });
			band = Rect.newSides(0,lowest.top,w.bounds.width,lowest.bottom);
			overlaps = bounds.select({ arg b; b.intersects(band) });

			minHeight = lowest.bottom;
			curry = band.top;
			currx = overlaps.maxValue({ arg b; b.right }) + hspacer;
		}); //  its all set if there are no views yet
	}

	remaining {
		^this.view.bounds.insetAll(0,curry,0,0)
	}

	*initClass {
		bgcolor = Color(0.886274509803, 0.94117647058824, 0.874509803921, 1);
		focuscolor = Color(1, 0.0980392156862, 0.129411764705, 1);
		hrcolor = Color(0.8862745098039, 0.901960784313, 0.819607843137, 1);
	}

}

