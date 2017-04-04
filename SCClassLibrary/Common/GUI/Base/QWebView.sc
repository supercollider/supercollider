QCallback : QObject {
	var <action;

	*qtClass { ^'QtCollider::QcCallback'; }

	*new {
		|func|
		^super.new.action_(func)
	}

	*newFrom {
		|other|
		if (other.isKindOf(QCallback)) {
			^QCallback(other.action)
		} {
			^QCallback(other)
		}
	}

	action_{
		|newAction|
		if (newAction != action) {
			this.prOnCalledSignals.do({
				|signal|
				if (action.notNil) { this.disconnectFunction(signal.asSymbol, action) };
				if (newAction.notNil) { this.connectFunction(signal.asSymbol, { |cb, v| newAction.(v) }, true) }
			});
			action = newAction;
		}
	}

	value {
		|...args|
		action.value(*args)
	}

	prOnCalledSignals {
		^this.methods(false, true, false).select({
			|sig|
			sig.asString.find("onCalled").notNil
		})
	}
}

WebView : View {
	classvar urlHandlers;

	var <onLoadFinished, <onLoadFailed, <onLoadProgress, <onLoadStarted, <onLinkActivated, <onLinkHovered, <onReloadTriggered, <onJavaScriptMsg,
	<onSelectionChanged, <onTitleChanged, <onUrlChanged, <onScrollPositionChanged, <onContentsSizeChanged, <onAudioMutedChanged,
	<onRecentlyAudibleChanged;

	*qtClass { ^'QtCollider::WebView'; }

	*initClass {
		urlHandlers = IdentityDictionary();
	}

	*setUrlHandler {
		|prefix, function|
		function = function.as(QCallback);
		this.prSetUrlHandler(prefix, function);
		urlHandlers[prefix.asSymbol] = function;
	}

	*prSetUrlHandler {
		|prefix, function|
		_Qt_SetUrlHandler
		^this.primitiveFailed
	}

	*clearCache {
		_QWebView_ClearMemoryCaches
		^this.primitiveFailed
	}

	init {
		this.connectMethod('interpret(QString)', 'onInterpret');
		this.connectMethod('renderProcessTerminated(int,int)', 'onTerminated');
	}

	setFontFamily {
		|which, font|
		this.invokeMethod('setFontFamily', [QWebFontFamily(which), font]);
	}

	triggerPageAction {
		|action, checked=true|
		this.invokeMethod('triggerPageAction', [QWebPageAction(action), checked])
	}

	setHtml {
		|html, baseUrl="sc:///"|
		this.invokeMethod('setHtml', [html, baseUrl]);
	}

	toHtml {
		|cb|
		this.invokeMethod('toHtml', [cb.as(QCallback)]);
	}

	toPlainText {
		|cb|
		this.invokeMethod('toPlainText', [cb.as(QCallback)]);
	}

	setContent {
		|data, mimeType="text/html", baseUrl="sc:///"|
		if (data.isKindOf(Int8Array).not) { Error("data must be an Int8Array").throw };
		this.invokeMethod('setContent', [data, mimeType, baseUrl]);
	}

	runJavaScript {
		|javascript, cb|
		this.invokeMethod('runJavaScript', [javascript, cb.as(QCallback)], false);
	}

	setAttribute {
		|attribute, on|
		this.invokeMethod('setWebAttribute', [QWebAttribute(attribute), on]);
	}

	testAttribute {
		|attribute|
		^this.invokeMethod('testWebAttribute', [QWebAttribute(attribute)]);
	}

	resetAttribute {
		|attribute|
		^this.invokeMethod('resetWebAttribute', [QWebAttribute(attribute)]);
	}

	findText {
		|text, reversed=false, cb=({})|
		this.invokeMethod('findText', [text, reversed, cb.as(QCallback)]);
	}

	onLinkActivated_ {
		|func|
		this.manageFunctionConnection( onLinkActivated, func, 'navigationRequested(QUrl, int, bool)' );
		onLinkActivated = func;
	}

	onReloadTriggered_{
		|func|
		this.manageFunctionConnection( onReloadTriggered, func, 'reloadTriggered(QString)' );
		onReloadTriggered = func;
	}

	onInterpret {
		|code|
		code.interpret();
	}

	onLoadStarted_{
		|func|
		this.manageFunctionConnection( onLoadStarted, func, 'loadStarted()' );
		onLoadStarted = func;
	}

	onLoadProgress_{
		|func|
		this.manageFunctionConnection( onLoadProgress, func, 'loadProgress(int)' );
		onLoadProgress = func;
	}

	onLoadFinished_ { arg func;
		case
		{ (func ? onLoadFailed).notNil && (onLoadFinished ? onLoadFailed).isNil }
		{ this.connectMethod( 'loadFinished(bool)', 'prLoadFinished' ); }
		{ (func ? onLoadFailed).isNil && (onLoadFinished ? onLoadFailed).notNil }
		{ this.disconnectMethod( 'loadFinished(bool)', 'prLoadFinished' ); };

		onLoadFinished = func;
	}

	onLoadFailed_ { arg func;
		case
		{ (func ? onLoadFinished).notNil && (onLoadFinished ? onLoadFailed).isNil }
		{ this.connectMethod( 'loadFinished(bool)', 'prLoadFinished' ); }
		{ (func ? onLoadFinished).isNil && (onLoadFinished ? onLoadFailed).notNil }
		{ this.disconnectMethod( 'loadFinished(bool)', 'prLoadFinished' ); };

		onLoadFailed = func;
	}

	prLoadFinished { arg ok;
		if(ok) { onLoadFinished.value(this) } { onLoadFailed.value(this) };
	}

	onLinkHovered_{
		|func|
		this.manageFunctionConnection( onLinkHovered, func, 'linkHovered(QString)' );
		onLinkHovered = func;
	}

	onSelectionChanged_{
		|func|
		this.manageFunctionConnection( onSelectionChanged, func, 'selectionChanged()' );
		onSelectionChanged = func;
	}

	onTerminated {
		|status, code|
		if (status != QRenderProcessTerminationStatus.normalTerminationStatus) {
			"QT Web Engine process terminated with code: %".format(code).error;
		}
	}

	onTitleChanged_{
		|func|
		this.manageFunctionConnection( onTitleChanged, func, 'titleChanged(QString)' );
		onTitleChanged = func;
	}

	onUrlChanged_{
		|func|
		this.manageFunctionConnection( onUrlChanged, func, 'urlChanged(QUrl)' );
		onUrlChanged = func;
	}

	onScrollPositionChanged_{
		|func|
		this.manageFunctionConnection( onScrollPositionChanged, func, 'scrollPositionChanged(QPointF)' );
		onScrollPositionChanged = func;
	}

	onContentsSizeChanged_{
		|func|
		this.manageFunctionConnection( onContentsSizeChanged, func, 'contentsSizeChanged(QSizeF)' );
		onContentsSizeChanged = func;
	}

	onAudioMutedChanged_{
		|func|
		this.manageFunctionConnection( onAudioMutedChanged, func, 'audioMutedChanged(bool)' );
		onAudioMutedChanged = func;
	}

	onRecentlyAudibleChanged_{
		|func|
		this.manageFunctionConnection( onRecentlyAudibleChanged, func, 'recentlyAudibleChanged(bool)' );
		onRecentlyAudibleChanged = func;
	}

	zoom					{ 			^this.getProperty('zoom') }
	zoom_					{ |zoom| 	this.setProperty('zoom', zoom) }

	hasSelection			{ 			^this.getProperty('hasSelection') }

	selectedText			{ 			^this.getProperty('selectedText') }

	title					{ 			^this.getProperty('title') }

	requestedUrl			{			^this.getProperty('requestedUrl') }

	url						{ 			^this.getProperty('url') }
	url_					{ |url| 	this.setProperty('url', url) }

	enterInterpretsSelection { 			^this.getProperty('enterInterpretsSelection') }
	enterInterpretsSelection_{ |b| 		this.setProperty('enterInterpretsSelection', b) }

	editable				{ 			^this.getProperty('editable') }
	editable_				{ |b| 		this.setProperty('editable', b) }

	pageBackgroundColor		{ 			^this.getProperty('backgroundColor') }
	pageBackgroundColor_	{ |color| 	this.setProperty('backgroundColor', color) }

	contentsSize			{ 			^this.getProperty('contentsSize') }

	scrollPosition			{ 			^this.getProperty('scrollPosition') }

	audioMuted				{ 			^this.getProperty('audioMuted') }
	audioMuted_				{ |muted| 	this.setProperty('audioMuted', muted) }

	delegateNavigation		{ 			^this.getProperty('delegateNavigation') }
	delegateNavigation_		{ |b| 		this.setProperty('delegateNavigation', b) }

	back 					{ this.triggerPageAction(\back) }
	forward					{ this.triggerPageAction(\forward) }

	// url { ^this.getProperty( \url ); }
	//
	// url_ { arg address; this.setProperty( \url, address ); }
	//
	// title { ^this.getProperty( \title ); }
	//
	// // Get the displayed content in html form.
	// html { ^this.getProperty( \html ); }
	//
	// // Set html content.
	// html_ { arg htmlString;
	// 	this.invokeMethod( \setHtml, htmlString );
	// }
	//
	// selectedText { ^this.getProperty( \selectedText ); }
	//
	// // Try to extract plain text from html content and return it.
	// plainText { ^this.getProperty( \plainText ); }
	//
	// reload { this.invokeMethod( 'reload' ); }
	//
	// back { this.invokeMethod( 'back' ); }
	//
	// forward { this.invokeMethod( 'forward' ); }
	//
	// findText { arg string, reverse = false;
	// 	this.invokeMethod( \findText, [string, reverse] );
	// }
	//
	// evaluateJavaScript { arg script;
	// 	this.invokeMethod( \evaluateJavaScript, script );
	// }
	//
	// // The given function will be evaluated when a page has loaded successfully.
	// // The calling WebView object is passed to the function.
	//
	//
	// // After this method is called with a function as an argument, WebView will not
	// // handle links in any way. Instead, the given function will be evaluated whenever
	// // the user activates (clicks) a link.
	//
	// // The argument passed to the function is the calling WebView object.
	//
	// // If this method is called with nil argument, WebView link handling will be
	// // restored again.
	//
	//
	// // After this method is called with an object as an argument, WebView will do
	// // nothing when page reload is requested. Instead, the given object's 'value' method
	// // will be called on such event.
	//
	// // The arguments passed to the 'value' method are this WebView instance and
	// // a String for the requested URL to be reloaded.
	//
	// // If this method is called with nil argument, WebView's page reload handling will
	// // be restored again.
	//
	// onReload_ { arg func;
	// 	this.manageFunctionConnection( onReload, func, 'reloadTriggered(QString)' );
	// 	this.setProperty( \delegateReload, func.notNil );
	// 	onReload = func;
	// }
	//
	// onJavaScriptMsg_ { arg func;
	// 	this.manageFunctionConnection( onJavaScriptMsg, func,
	// 	'jsConsoleMsg(const QString&, int, const QString&)' );
	// 	onJavaScriptMsg = func;
	// }
	//
	// enterInterpretsSelection { ^this.getProperty( \enterInterpretsSelection ); }
	//
	// enterInterpretsSelection_ { arg bool;
	// 	this.setProperty( \enterInterpretsSelection, bool );
	// }
	//
	// editable { ^this.getProperty( \editable ); }
	//
	// editable_ { arg bool;
	// 	this.setProperty( \editable, bool );
	// }
	//
	// // Set a specific font family to be used in place of a CSS-specified generic font family.
	// // The 'generic' argument must be one of the following symbols:
	// // \standard, \fixed, \serif, \sansSerif, \cursive, \fantasy
	//
	// setFontFamily { arg generic, specific;
	// 	this.invokeMethod( \setFontFamily, [QWebFontFamily(generic), specific] )
	// }

	//---------------------------------- private --------------------------------------//

}

/*
perhaps also:
- history  // returning a string list of urls.
*/
