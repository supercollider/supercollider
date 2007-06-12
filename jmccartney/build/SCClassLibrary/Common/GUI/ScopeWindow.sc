{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red191\green0\blue0;\red191\green0\blue0;}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf2 // These classes are very likely to change.\
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural
\cf0 \
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural
\cf2 //\
//ScopeWindow : GUIScreen \{\
//	var <>scopeView;\
//	*new \{ arg bank, index, name, bounds, refreshRate = 0.05, size = 512;\
//		var view, scopeView, innerBounds;\
//		innerBounds = bounds.moveTo(0,0);\
//		scopeView = this.viewClass.new(Pen(foreColor: rgb(255,255,0)),innerBounds, bank, index);\
//		scopeView.zoom = ceil( size / bounds.width );\
//		view = View(Pen(foreColor: Color.black, action: \\fill), [ innerBounds, scopeView ]);\
//		^super.new(name, bounds, view).initScopeWindow(bank, index, scopeView, refreshRate);\
//	\}\
//	initScopeWindow \{ arg bank, index, argScopeView, refreshRate;\
//		scopeView = argScopeView;\
//		this.sched(0, inf, \{ this.refresh; refreshRate \});\
//	\}\
//	*viewClass \{ ^ScopeView \}\
//	fitZoom \{ scopeView.fitZoom \}\
//\}\
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural
\cf0 \
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural
\cf3 //\
//XYScopeWindow : ScopeWindow \{\
//	*viewClass \{ ^XYScopeView \}\
//\}\
//\
//SignalWindow : ScopeWindow \{\
//	*viewClass \{ ^SignalView \}\
//\}\
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural
\cf0 \
}