
PageLayout  {

	classvar <>screenWidth = 1100, <>screenHeight = 700; // tibook
	classvar <>bgcolor ,<>focuscolor,<>hrcolor ;
	
	var windows, <>vspacer=3,<>hspacer=5, maxx=0, maxy=0, currx, curry;
	var <margin; // max area in window we may occupy
	var <winMaxBounds,tabs=0;
	var onBoundsExceeded='newWindow';
	var <>isClosed=false;
	
	var autoRemoves;
	
	*new{ arg title,width,height ,posx,posy,hspacer,vspacer;
		^super.new.init(title,width ? screenWidth,height ? screenHeight,
							posx ? 20,posy ? 20,hspacer ? 5,vspacer ? 3)
	}

	// see also .within
	*newWithin { arg w,rect;
		^super.new.initWithin(w,rect);
	}
	
	//asPageLayout
	*guify { arg lay,title,width,height;
		if(lay.class === GUIPlacement,{ ^lay });
		if(lay.notNil and: {lay.checkNotClosed},{
			// if its a GUIWindow we return a flow layout on it
			if(lay.isKindOf(SCWindow),{
				^this.onWindow(lay)
			},{
				^lay
			});
		},{
			^this.new(title,width,height )
		})
	}
	
	init { arg title,width,height,posx,posy,arghspacer,argvspacer;
		var w,gwmaxx,gwmaxy;
		hspacer = arghspacer;
		vspacer = argvspacer;
		/* attempt to find a suitable position
		if(posx.isNil,{
			gwmaxx = GUIWindow.allWindows
				.maxItem({ arg win; win.bounds.right }).bounds.right;
			posx = (gwmaxx + x).wrap(0,screenWidth) - x;
		});
		if(posy.isNil,{
			gwmaxy = GUIWindow.allWindows
				.maxItem({ arg win; win.bounds.bottom }).bounds.bottom;
			posy = (gwmaxy + y).wrap(0,screenHeight) - y;
		});
		*/
		windows=windows.add
		(	
			w=SCWindow.new("< " ++ title.asString ++ " >",
				Rect.new( posx, posy, width, height ) )
			.onClose_({  	
						this.close; // close all windows in this layout
					})
		);
		w.view.background_(bgcolor);
		//.focusColor_(focuscolor)	

		isClosed = false;	
			
		margin=Rect(0 + hspacer,0 + vspacer,
				width - (hspacer * 2),height - (vspacer * 2));
		currx=margin.left;
		curry=margin.top;
		
		autoRemoves = List.new;
		this.front;
	}
	
	window { ^windows.last }
	asView { ^this.window.view }
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
			curry=curry+maxy+vspacer;
			maxy=0;
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
			
		if((x+currx)>maxx,{maxx=x});
		if(y>maxy,{maxy=y});
		
		rect=Rect.new(currx,curry,x,y);
		currx=currx+x+(argSpacer?hspacer);		
		^rect
	}
	
	layDown { arg x, y, sp;
		var rect;
		if(x>maxx,{maxx=x});
		if(y>maxy,{maxy=y});
		rect=Rect.new(currx,curry,x,y);
		curry=curry + y + (sp ? vspacer);
		^rect	
	}
	
	
	// act like a GUIWindow 
	checkNotClosed { ^isClosed.not }
	front { windows.reverseDo({arg w; w.front }); }
	close { // called when the GUIWindow closes
		if(isClosed.not,{
			isClosed = true; 
			if(windows.notNil,{
				windows.do({ arg w; 
					w.close; 
				});
				windows=nil;
			});
			autoRemoves.do({ arg updater; updater.remove });
			NotificationCenter.notify(this,\didClose);
		})
	}

	hr { arg color,height=8,borderStyle=1; // html joke
		var r;
		this.startRow;
		r=this.layRight(this.margin.width - (2 * hspacer), height,0);
		// on window resize, jmc sets every view to a minimum of 8
		SCStaticText(this.window,r).string_("").background_(color ? hrcolor)
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
	focusOnAButton {
		var first;
		// not needed now ?
		first = this.window.views.detect({arg v; v.isKindOf(SCButton) });
		if(first.notNil,{first.focus });
	}

	backColor { ^this.window.view.background }
	backColor_ { arg c; this.window.view.background_(c) }
	
	removeOnClose { arg dependant;
		autoRemoves = autoRemoves.add(dependant);
	}
	
	// place the code posted by gui builder onto flow layout
//	placeCode { arg function;
//		var win,lastView,newViews,rect;
//		win = this.window;
//		lastView = win.views.size;
//		function.value(win);
//		newViews = win.views.copyRange(lastView,win.views.size - 1);
//		
//		rect = this.layRight(
//			newViews.maxValue({ arg v; v.bounds.right }),
//			newViews.maxValue({ arg v; v.bounds.bottom })
//		);
//		newViews.do({ arg v;
//			v.bounds_(v.bounds.moveBy(rect.left,rect.top));
//			//v.prSetBounds(v.bounds.moveBy(rect.left,rect.top));
//		});
//		win.refresh;
//	}

	// create a layout within the current layout
	within { arg x,y,func;  		
		var l,r;
		r=this.layRight(x,y);
		l=this.class.newWithin(this.window,r);
		func.value(l);
	}
	
//	withinBox { arg x,y,func; // experimental
//		var l,r,w;
//		w = this.window;
//		r=this.layRight(x,y);
//	
//		StringView(w,r,"").prSetBorderStyle(3);
//
//		l=this.class.newWithin(w,r.insetAll(hspacer,vspacer,hspacer,vspacer));
//	
//		func.value(l);
//	}

	initWithin { arg w,rect;
		windows=windows.add(w);
		margin=rect;
		currx=margin.left;
		curry=margin.top;
		autoRemoves = List.new;
	}

	resizeWindowToFit { 
		// cascade all open windows and shrink them to contents
		// margin is still set large
		windows.reverse.do({ arg w;
			var b;
			
			b = Rect.new(w.bounds.left, w.bounds.top,
				 w.view.children.maxValue({ arg v; v.bounds.right }) 
				 		+ hspacer ,
				 w.view.children.maxValue({ arg v; v.bounds.bottom }) 
				 		+ vspacer);
			
			// SCWindow.bounds_ doesn't work yet	 
			//w.bounds_(b);
			margin = b.insetAll(hspacer,vspacer,hspacer,vspacer);
			// warning: subsequent windows would have the same margin
			w.front;
		})
	}
	
	warnError {
		"FlowLayout within a FlowLayout exceeded bounds !!".warn;
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
				vspacer);
		this.window.view.background_(ow.view.background);
		tabs.do({
			this.layRight(10,10);
		});
	}

	// make flowLayout gui objects dropable
	*onWindow { arg w;
		^this.newWithin(w,w.bounds.insetAll(5,5,5,5)).findPosition;
	}
	findPosition {	
		var w;
		var bounds,lowest,band,overlaps;
		w = this.window;
		bounds = w.views.collect({ arg v; v.bounds });
		if(bounds.notEmpty,{
			lowest = bounds.maxItem({ arg b; b.bottom });
			band = Rect.newSides(0,lowest.top,w.bounds.width,lowest.bottom);
			overlaps = bounds.select({ arg b; b.intersects(band) });
			
			maxy = lowest.bottom;
			curry = band.top;
			currx = overlaps.maxValue({ arg b; b.right }) + hspacer;
		}); //  its all set if there are no views yet
	}

	
	// making FlowLayout compat with LayoutView
//	add { arg view;
//		view.bounds = this.layRight(view.bounds.width,view.bounds.height);
//		// view.window
//		// items = items.add(view);
//		// if view.window != this.window, move it
//		
//		^true
//	}

	*initClass {
		bgcolor = Color(0.886274509803, 0.94117647058824, 0.874509803921, 1);
		focuscolor = Color(1, 0.0980392156862, 0.129411764705, 1);
		hrcolor = Color(0.8862745098039, 0.901960784313, 0.819607843137, 1);
	}

}


GUIPlacement { // this adapts flowLayout code so that it can work with gui builder positioned objects
	var <>window,<>bounds;
	*new { arg window,bounds;
		^super.new.window_(window).bounds_(bounds)
	}
	layRight {  ^bounds } // liar
	margin { ^window.bounds }
	storeParamsOn { arg stream;
		stream << "( w," <<< bounds << ")";
	}
}
