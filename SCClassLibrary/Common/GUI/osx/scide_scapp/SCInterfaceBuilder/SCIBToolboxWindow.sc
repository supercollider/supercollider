/*
	SCIBToolboxWindow()

	select view(s) and apple-drag between windows to copy
	select view(s) and apple-drag inside a window, or press c, to copy
	press delete to delete

	by scott micheli-smith
	sclang integration by jan trutzschler
*/
SCIBPanelWindow
{
	var <window,<userview,>parent,isSelected=false;

	var selection,<selectedViews,views;

	var <gridStep = 10,<gridOn = false,dragging,indent,multipleDragBy;

	var resizeHandles,resizeFixed, dropX, dropY;


	*new { |window, bounds| ^super.new.init(window, bounds) }


	init { |win, bounds|
		window = win;

		views = window.view.children ? Array.new;
		this.makeUserview;
		window.constructionView = userview;

	}

	gridOn_ { |bool|
		gridOn = bool;
		window.refresh;
	}

	gridStep_ { |step|
		gridStep = step;
		this.debug(gridStep);
		if(gridOn, { window.refresh })
	}

	makeUserview {
		var w = window.bounds.width, h = window.bounds.height;

		userview !? { userview.remove };
		userview = SCConstructionView(window,Rect(0,0,w,h)).resize_(5);

		userview.beginDragAction = {
			var classes,rects;

			if ( selectedViews.size > 0,{
				#classes, rects = flop(selectedViews.collect({ |view|
					[ view.class, view.bounds ]
				}));

				SCIBMultipleDrag(classes,rects)
			})
		};

		userview.keyDownAction = { |v,c,m,u| this.panelSelect.keyDown(c,u) };
		userview.mouseDownAction = { |v,x,y| this.panelSelect.mouseDown(x,y) };
		userview.mouseUpAction = { |v,x,y| this.panelSelect.mouseUp };

		userview.mouseOverAction = { |v,x,y| dropX = x; dropY = y };

		userview.canReceiveDragHandler = {
			SCView.currentDrag.isKindOf( SCIBDrag )
		};

		userview.receiveDragHandler = {
			var addedViews = Array.new;

			SCView.currentDrag.do({ |class, rect|
				rect = rect.moveBy( dropX, dropY );
				addedViews = addedViews.add( class.paletteExample(window,rect).enabled_(false) );
			});

			views = views.addAll( addedViews );

			dragging = true;
			selectedViews = addedViews;
			indent = dropX@dropY - views.last.bounds.origin;

			this.makeUserview.updateResizeHandles;
			window.front.refresh;

			this.panelSelect
		};

		userview.mouseMoveAction = { |v,x,y| this.drag(x,y) };
		userview.focus;

		this.initDrawFunc
	}

	initDrawFunc {
		userview.drawFunc = {
			var b,n,h,w;

			if(gridOn,{
				b = window.view.bounds;
				h = b.height;
				w = b.width;

				Color.yellow(1,0.4).set;

				n = h / gridStep;
				(n-1).do({ |i| i=i+1*gridStep;
					Pen.moveTo(0@i).lineTo(w@i).stroke;
				});

				n = w / gridStep;
				(n-1).do({ |i| i=i+1*gridStep;
					Pen.moveTo(i@0).lineTo(i@h).stroke;
				})
			});

			Color.blue.set;

			if(isSelected,{
				Pen.width_(4).strokeRect(window.bounds.moveTo(0,0));
			});

			selectedViews.do({ |v|
				Pen.strokeRect(v.bounds)
			});
			Pen.width_(1);

			resizeHandles.do({ |r|
				Pen.fillRect(r)
			});

			if(selection.notNil, {
				Pen.strokeRect(selection.rect)
			});

		}
	}

	deselect {
		isSelected = false;
		window.refresh
	}

	updateResizeHandles { var r,d=4;
		resizeHandles = if( selectedViews.size == 1,{
			r = selectedViews.first.bounds;
			[ r.leftTop, r.rightTop, r.rightBottom, r.leftBottom ]
				.collect({ |center| Rect.aboutPoint(center,d,d) })
		});
		window.refresh
	}

	panelSelect {
		isSelected = true;
		if(parent.notNil,
			{ parent.panelSelect(this) })
	}


	setResizeFixed { |resizeHandle|
		var r = selectedViews.first.bounds,i = resizeHandles.indexOf(resizeHandle);
		resizeFixed=r.perform([ \rightBottom, \leftBottom, \leftTop, \rightTop ][i])
	}


	mouseDown { |x,y|
		var view,p,handle;

		p = x@y;

		if( resizeHandles.notNil and: {
			(handle = resizeHandles.detect({ |h| h.containsPoint(p) }) ).notNil
		},
		{
			this.setResizeFixed(handle)
		},
		{
			resizeFixed = nil;
			view = this.viewContainingPoint(p);

			dragging = view.notNil;

			if( dragging, {
				indent = p - view.bounds.origin;

				if( (selectedViews.size > 1) and:
					{ selectedViews.includes(view) },
				{
					multipleDragBy = view
				},
				{
					multipleDragBy = nil;
					selectedViews = [ view ]
				})
			},{
				selectedViews = [];
				selection = SCIBAreaSelection(p)
			})
		});
//		this.debug(selectedViews);
		this.updateResizeHandles
	}

	drag { |x,y|
		var view,f,p=x@y;
		if( dragging, {

			if( resizeFixed.isNil,
			{
				if(multipleDragBy.notNil,
				{
					f = p - ( multipleDragBy.bounds.origin + indent );

					selectedViews.do({ |v|
						this.quantSetBounds(v,v.bounds.moveBy(f.x,f.y))
					})
				},{
					view = selectedViews.first;
					this.quantSetBounds(view,view.bounds.moveToPoint(p-indent));

					this.updateResizeHandles
				})
			},{
				if(gridOn,{ p = p.round(gridStep) });
				selectedViews.first.bounds = Rect.fromPoints(p,resizeFixed);
				this.updateResizeHandles
			})
		},
		{
			selection.mouseDrag(p);
			selectedViews = views.select({ |view|
				selection.selects(view.bounds)
			});
			window.refresh
		})
	}

	mouseUp { |x,y|
		if(selection.notNil,{
			selection = nil; window.refresh
		})
	}

	keyDown { |c,u|
		var newViews;
		case (
		// delete
		{u==127}, {
			if(selectedViews.isEmpty.not,{
				selectedViews.do({ |v|
					views.remove(v.remove);
				});
				selectedViews=[];

				this.updateResizeHandles
			})
		},
		// clone
		{(c==$c) or: (c==$C)},{
			if(selectedViews.isEmpty.not,{
				newViews=selectedViews.collect({ |v|
					v.class.paletteExample(window,v.bounds.moveBy(40,40))
				});
				views=views++newViews;
				selectedViews=newViews;

				this.makeUserview.updateResizeHandles
			})
		})
	}


	quantSetBounds { |view,rect|
		view.bounds=if(gridOn,
			{ rect.moveToPoint(rect.origin.round(gridStep)) },
			{ rect })
	}


	viewContainingPoint { |point|
//		this.debug(views);
		views.do({ |view|
			if(view.bounds.containsPoint(point),
				{ ^view })
		})
		^nil
	}

	asCompileString {
		var str = "";
		views.do({ |v| str = str ++ format("%.new(w,%);\n",v.class,v.bounds) });
		^format( "(\nvar w = SCWindow.new(\"\",%).front;\n%\n)",window.bounds,str )
	}

}

SCIBToolboxWindow
{
	classvar shared;
	var <window,viewPallatte,panels,selectedPanel, <gridStep=20, gridOn=false;

	*front{
		if(shared.isNil){
			shared = SCIBToolboxWindow.new;
		}{
			shared.window.front;
		};
		^shared
	}

	*new { ^super.new.init }

	addWindow{|win|
		var panel;
		panel = SCIBPanelWindow.new(win).parent_(this);
		win.bounds = win.bounds.moveTo(window.bounds.left+window.bounds.width,window.bounds.height);
//		win.view.children.do{|it| it.enabled_(false)};
		panels = panels.add(panel);
		this.debug(this.gridStep);
		panel.gridOn_(gridOn).gridStep_(this.gridStep);
	}

	removeWindow{|win|
		var panel = SCIBPanelWindow.new(win).parent_(this), indx;
		win.view.children.copy.do{|it|
			it.enabled_(true);
			if(it.isKindOf(SCConstructionView)){it.remove;}
		};
		indx = this.findPanelIndexForWindow(win);
		if(indx.notNil){
			panels.removeAt(indx);
		}
	}

	findPanelIndexForWindow{|win|
		panels.do{|it,i|
			if(it.window == win){^i}
		};
		^nil
	}

	enableGridWithSize{|flag, gridsize|
		gridOn = flag;
		gridStep = gridsize;
		panels.do({ |panel|
			panel.gridOn_(gridOn).gridStep_(gridStep);
		})
	}

	init
	{
		var vh = 24, vw = 260,gridNB,gridBut;

		var height = 20 + 4 * (vh + 2) +2, os=0;
		var vw2 = div(vw,2);

		var funcButCol = Color.blue;

		window = SCWindow("IB",Rect(50,800,vw+8,height)).front
					.onClose_({shared=nil});
		window.view.decorator = FlowLayout(window.view.bounds);

		panels = Array.new;

		SCButton(window,Rect(2,os,vw,vh)).states_([["New Window",nil,funcButCol]])
			.canFocus_(false).action = {
				this.addWindow(SCWindow("pane",Rect(100,100,400,400)).front)
			};

		SCButton(window,Rect(2,os,vw,vh)).states_([ ["-> Code",nil,funcButCol]])
			.canFocus_(false).action_{ if ( selectedPanel.notNil,
				{
				Document("window construction code", selectedPanel.asSCIBCompileString);
  			} )
			};

		SCButton(window,Rect(2,os,vw,vh)).states_([ ["Toggle Edit",nil,funcButCol]])
			.canFocus_(false).action = {
				selectedPanel.window.toggleEditMode;
			};

		gridBut = SCButton(window,Rect(2,os,vw2-8,vh))
			.canFocus_(false).action_{ |v|
				gridNB.visible = v.value == 1;
				this.enableGridWithSize(v.value == 1, gridNB.value);

			}
			.states_([["Q On",nil,funcButCol],["Q Off",nil,funcButCol]]);

		gridNB = SCNumberBox(window,Rect(2+vw2,os,vw2,vh))
			.action_{ |v|
				v.value = v.value.asInt.clip(3,40);
				panels.do({ |panel| panel.gridStep = v.value })
			}
			.align_(\center).value_(10).visible_(false);

		viewPallatte = SCIBViewPallatte(window,Rect(2, 2, vw, (height - window.view.decorator.top)));

	}

	panelSelect { |panel|
		if( panel !== selectedPanel,{
			if(selectedPanel.notNil,{ selectedPanel.deselect });
			selectedPanel = panel
		})
	}
}

SCIBViewPallatte
{
//	classvar <viewList;
	var showExamples = false;

	*new { |window,rect,parent|
		^super.new.init(window,rect,parent)
	}

	init { |window,rect,parent|
		var x = rect.top, y = rect.left;
		var bW = rect.width, bH = rect.height, list, scroll;
		bH.postln;
		list = SCView.allSubclasses.reject{|it|
			(it.superclasses.indexOf(SCContainerView).notNil
			or: (it.name === 'SCContainerView')
			or: (it.name ==='SCStaticTextBase')
			or: (it.name === 'SCSliderBase')
			or: (it.name === 'SCControlView')
			or: (it.name === 'SCDragView'))
		};
		scroll = SCScrollView(window, Rect(0,0,bW-2, bH-34))
					.resize_(5);
		scroll.decorator = FlowLayout(scroll.bounds.moveTo(0,0));
		list.do({ |class,i|
			var drag = SCIBPallatteDrag(class,Rect(0,0,100,20));
			SCDragSource(scroll,((bW*0.5)-12)@24)
				.string_(class.asString).align_(\center).object_(drag);
			if(showExamples){
				try{
					class.paletteExample(scroll, Rect(0,0,(bW*0.5)-12,24));
				}{
					"no paletteExample found".warn;
				};
				scroll.decorator.nextLine;
			};

		})
	}

	//*initClass {
//		viewList = [
//			SCButton,
//			SCStaticText,
//			SCNumberBox,
//			SCSlider,
//			SCRangeSlider,
//			SCMultiSliderView,
//			SCPopUpMenu,
//			SC2DTabletSlider,
//			SC2DSlider,
//			SCTabletView,
//			SCEnvelopeView,
//			SCDragBoth,
//			SCDragSink,
//			SCDragSource,
//			Knob,
//			SCTextView,
//			//SCMovieView
//		];
//	}
}

SCIBAreaSelection
{
	var click,round,<rect;

	*new { |p,r| r !? { p = round(p,r) };
		^super.newCopyArgs(p,r).mouseDrag(p)
	}

	mouseDrag { |drag|
		round !? { drag = round(drag,round) };
		rect = Rect.fromPoints(click,drag)
	}

	selects { |aRect|
		^rect.intersects(aRect)
	}
}


SCIBDrag { }

SCIBMultipleDrag : SCIBDrag
{
	var classes,rects,minX, minY;

	*new { |classes, rects|
		^super.newCopyArgs( classes, rects ).init
	}

	init {
		minX = inf;
		minY = inf;
		rects.do({ |r|
			if ( r.left < minX, { minX = r.left });
			if ( r.top < minY, { minY = r.top })
		});
		minX = minX.neg;
		minY = minY.neg;
	}

	do { |func|
		classes.do({ |class,i|
			func.( class, rects[ i ].moveBy( minX, minY ), i )
		})
	}
}

SCIBPallatteDrag : SCIBDrag
{
	var class, rect;

	*new { |class, rect|
		^super.newCopyArgs( class, rect )
	}

	do { |func| func.(class,rect,0) }

	asString { ^class.asString }
}
