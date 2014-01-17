// these were the QT implementation classes.
// These subclasses should be removed after a while

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
QSlider : Slider {}
QSlider2D : Slider2D {}
QSoundFileView : SoundFileView {}
QPopUpMenu : PopUpMenu {}
QCheckBox : CheckBox {}
QFont : Font {}
QTreeViewItem : TreeViewItem {}
QImage : Image {}
QView : View {}

// keeping the names of older versions to avoid breaking too much code

CompositeView : View {}

PlusFreqScopeView : FreqScopeView {}
PlusFreqScope : FreqScopeView {}

FreqScopeWindow : FreqScope {}
PlusFreqScopeWindow : FreqScope {}

Scope2 : ScopeView {}
QScope : SCScope {}
QScope2 : ScopeView {}


+ Object {
	// this isn't really needed, but some third party may be relying on it.
	*implClass { ^this }

}

// these were already removed, but it is better to deprecate them
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
