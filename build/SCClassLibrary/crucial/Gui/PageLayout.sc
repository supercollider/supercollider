
MultiPageLayout  {

	classvar <>screenWidth = 900, <>screenHeight = 700; // tibook
	classvar <>bgcolor;
	
	var windows,views;
	var onBoundsExceeded='newWindow',metal;
	var <>isClosed=false;
	
	var autoRemoves;
	
	*new { arg title,width,height ,posx,posy,hspacer,vspacer,metal=true;
		^super.new.init(title,width ,height ,
							posx ,posy ,hspacer ,vspacer , 
							metal)
	}
	
	init { arg title,width,height,posx,posy,arghspacer=3,argvspacer=3,argmetal=true;
		var w;
		windows=windows.add
		(	
			w=SCWindow.new("< " ++ title.asString ++ " >",
				Rect.new( posx ? 20, posy ? 20, width ? screenWidth, height ? screenHeight) )
			.onClose_({  	
						this.close; // close all windows in this layout
					})
		);
		metal = argmetal;
		if(metal.not,{
			w.view.background_(bgcolor);
		});
		isClosed = false;
		views = views.add( FlowView(w ) );
		autoRemoves = IdentitySet.new;
	}
	
	window { ^windows.last }
	view { ^views.last }
	asView { ^this.view }
	//asView {}
	add { arg view;
		if(this.view.wouldExceedBottom(view.bounds),{
			this.newWindow;
		});
		this.view.add(view);
	}
	asPageLayout { arg name,width,height,x,y;
		if(isClosed,{
			^this.class.new(name,width,height,x,y)
		},{
			^this
		})
	}
	startRow { 
		this.view.startRow;
	}
	
	*guify { arg lay,title,width,height,metal=false;
		^lay ?? {
			this.new(title,width,height,metal: metal )
		}
	}
	
	// act like a GUIWindow 
	checkNotClosed { ^isClosed.not }
	front {
		//windows.do({ arg w; w.unhide });
		windows.reverseDo({arg w; w.front }); 
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
				windows=views=nil;
			});
			autoRemoves.do({ arg updater; updater.remove });
			NotificationCenter.notify(this,\didClose);
		});
	}

	hr { arg color,height=8,borderStyle=1; // html joke
		this.view.hr;
	}
//	tab {
//		this.layRight(10,10);
//	}
	indent { arg by = 1;
		//tabs = max(0,tabs + by);
	}
	layRight { arg w,h; ^Rect(0,0,w,h) }
//	width {
//		^margin.width - (tabs * 10)
//	}

	focus { arg index=0;
		var first;
		first = this.window.views.at(index);
		if(first.notNil,{first.focus });
	}

	//TODO remove
	backColor { ^this.view.background }
	backColor_ { arg c; this.view.background_(c) }
	
	background_ { arg b; this.view.background_(b) }
	
	
	removeOnClose { arg dependant;
		autoRemoves = autoRemoves.add(dependant);
	}
	
	resizeToFit { 
		// cascade all open windows and shrink them to contents
		// margin is still set large
		views.reverse.do({ arg v,vi;
			v.resizeToFit;
			// backwards 
			windows.at(vi).setInnerExtent(v.bounds.width, v.bounds.height );
		});
	}
	fullScreen {
		windows.reverse.do({ arg w;
			w.bounds = SCWindow.screenBounds;
				// .fullScreen   issues
		});
	}
	endFullScreen {
		windows.do({ arg w; w.endFullScreen })
	}
	warnError {
		"PageLayout within a PageLayout exceeded bounds !!".warn;
	}

	newWindow {
		var ow;
		ow = this.window;
		this.init("..." ++ ow.name,
				nil,
				nil,
				(ow.bounds.left + 100).wrap(0,900),
				(ow.bounds.top+10).wrap(0,300),
				nil,
				nil,
				metal);
		this.window.front;
	}
	*initClass {
		bgcolor = Color(0.886274509803, 0.94117647058824, 0.874509803921, 1);
	}
}




Sheet {
	*new { arg buildDialog,name="",x=100,y=100,width=600,height=600;
		var layout;
		layout = MultiPageLayout(name,width,height,x,y);
		//layout = FlowView(nil,Rect(x,y,width,height));
		buildDialog.value(layout);
		layout.resizeToFit;
		layout.front;
		^layout
	}
}

ModalDialog { // hit ok or cancel

	*new { arg buildDialog,okFunc,name="?",cancelFunc;
	
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
		
		},name);
	}

}




// for old usage
// to be deprec in favor of MultiPageLayout or FlowView
PageLayout  {

	// should get these from Cocoa
	classvar <>screenWidth = 1100, <>screenHeight = 700; // tibook
	
	classvar <>bgcolor ,<>focuscolor,<>hrcolor;
	
	var windows, <>vspacer=3,<>hspacer=5, maxx=0, maxy=0, currx, curry;
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
			autoRemoves.do({ arg updater; updater.remove });
			NotificationCenter.notify(this,\didClose);
		});
	}

	hr { arg color,height=8,borderStyle=1; // html joke
		var r;
		this.startRow;
		r=this.layRight(this.margin.width - (2 * hspacer), height,0);
		SCStaticText(this.window,r).string_("").background_(color ? hrcolor).resize_(2)
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

	//TODO remove
	backColor { ^this.view.background }
	backColor_ { arg c; this.view.background_(c) }
	
	background_ { arg b; this.view.background_(b) }
	
	
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
			
			maxy = lowest.bottom;
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



