QWindow : Window {}
QTopScrollWidget : TopScrollWidget {}
QScrollTopView : ScrollTopView {}
QScrollView : ScrollView {}
QScrollCanvas : ScrollCanvas {}
QTopView : TopView {}
QWebView : WebView {}
QStaticText : StaticText {}
QTextViewBase : TextViewBase {}
QItemViewBase : ItemViewBase {}
QAbstractScroll : AbstractScroll {}
QAbstractStepValue : AbstractStepValue {}
QHLayoutView : HLayoutView {}
QTextField : TextField {}
QButton : Button {}
QVLayoutView : VLayoutView {}
QUserView : UserView {}
QStethoscope : Stethoscope {}
QDragView : DragView {}
QDragSource : DragSource {}
QDragSink : DragSink {}
QDragBoth : DragBoth {}
QFileDialog : FileDialog {}
QDialog : Dialog {}
QEnvelopeView : EnvelopeView {}
QKnob : Knob {}
QTreeView : TreeView {}
QTextView : TextView {}
QLayout : Layout {}
QNumberBox : NumberBox {}
QLineLayout : LineLayout {}
QHLayout : HLayout {}
QVLayout : VLayout {}
QGridLayout : GridLayout {}
QStackLayout : StackLayout {}
QLevelIndicator : LevelIndicator {}
QListView : ListView {}
QMultiSliderView : MultiSliderView {}
QPen : Pen {}
QQuartzComposerView : QuartzComposerView {}
QRangeSlider : RangeSlider {}
QScope : Scope {}
QScope2 : Scope2 {}
QSlider : Slider {}
QSlider2D : Slider2D {}
QSoundFileView : SoundFileView {}
QPopUpMenu : PopUpMenu {}
QCheckBox : CheckBox {}
QFont : Font {}
QTreeViewItem : TreeViewItem {}
QImage : Image {}
QView : View {}


CompositeView : View {}

PlusFreqScopeView : FreqScopeView {}
PlusFreqScope : FreqScopeView {}

FreqScopeWindow : FreqScope {}
PlusFreqScopeWindow : FreqScope {}


// avoid breaking code
+ Object {
	*implClass { ^this }

}

+ Window {
	drawHook_ { |func|
		this.deprecated(thisMethod, this.class.findRespondingMethodFor('drawFunc_'));
		this.drawFunc_(func);
	}
}


+ UserView {
	drawHook_ { |func|
		this.deprecated(thisMethod, this.class.findRespondingMethodFor('drawFunc_'));
		this.drawFunc_(func);
	}
}
