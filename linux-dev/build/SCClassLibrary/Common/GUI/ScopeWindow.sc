// These classes are very likely to change.

//
//ScopeWindow : GUIScreen {
//	var <>scopeView;
//	*new { arg bank, index, name, bounds, refreshRate = 0.05, size = 512;
//		var view, scopeView, innerBounds;
//		innerBounds = bounds.moveTo(0,0);
//		scopeView = this.viewClass.new(Pen(foreColor: rgb(255,255,0)),innerBounds, bank, index);
//		scopeView.zoom = ceil( size / bounds.width );
//		view = View(Pen(foreColor: Color.black, action: \fill), [ innerBounds, scopeView ]);
//		^super.new(name, bounds, view).initScopeWindow(bank, index, scopeView, refreshRate);
//	}
//	initScopeWindow { arg bank, index, argScopeView, refreshRate;
//		scopeView = argScopeView;
//		this.sched(0, inf, { this.refresh; refreshRate });
//	}
//	*viewClass { ^ScopeView }
//	fitZoom { scopeView.fitZoom }
//}

//
//XYScopeWindow : ScopeWindow {
//	*viewClass { ^XYScopeView }
//}
//
//SignalWindow : ScopeWindow {
//	*viewClass { ^SignalView }
//}

