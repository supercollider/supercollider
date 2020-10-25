WebView : View {
	classvar urlHandlers;

	var <onLoadFinished, <onLoadFailed, <onLoadProgress, <onLoadStarted, <onLinkActivated, <onLinkHovered, <onReloadTriggered, <onJavaScriptMsg,
		<onSelectionChanged, <onTitleChanged, <onUrlChanged, <onScrollPositionChanged, <onContentsSizeChanged, <onAudioMutedChanged,
		<onRecentlyAudibleChanged, <enterInterpretsSelection = false;

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
		|func|
		this.invokeMethod('toHtml', [func.as(QCallback)]);
	}

	toPlainText {
		|func|
		this.invokeMethod('toPlainText', [func.as(QCallback)]);
	}

	setContent {
		|data, mimeType="text/html", baseUrl="sc:///"|
		if (data.isKindOf(Int8Array).not) { Error("data must be an Int8Array").throw };
		this.invokeMethod('setContent', [data, mimeType, baseUrl]);
	}

	runJavaScript {
		|javascript, func|
		this.invokeMethod('runJavaScript', [javascript, func.as(QCallback)], false);
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

	navigate {
		|url|
		^this.invokeMethod('navigate', [url]);
	}

	findText {
		|text, reversed=false, func=({})|
		this.invokeMethod('findText', [text, reversed, func.as(QCallback)]);
	}

	onLinkActivated_ {
		|func|
		this.manageFunctionConnection( onLinkActivated, func, 'navigationRequested(QUrl,int,bool)', true);
		onLinkActivated = func;
	}

	onReloadTriggered_{
		|func|
		this.manageFunctionConnection( onReloadTriggered, func, 'reloadTriggered(QString)' );
		onReloadTriggered = func;
	}

	// this function is called by a javascript callback: see enterInterpretsSelection below
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

	onJavaScriptMsg_ {
		|func|
		this.manageFunctionConnection( onJavaScriptMsg, func, 'jsConsoleMsg(const QString&, int, const QString&)' );
		onJavaScriptMsg = func;
	}

	zoom { ^this.getProperty('zoom') }
	zoom_ { |zoom| this.setProperty('zoom', zoom) }

	hasSelection { ^this.getProperty('hasSelection') }

	selectedText { ^this.getProperty('selectedText') }

	title { ^this.getProperty('title') }

	requestedUrl { ^this.getProperty('requestedUrl') }

	url { ^this.getProperty('url') }
	url_ { |url| this.setProperty('url', url) }

	enterInterpretsSelection_ { |b|
		enterInterpretsSelection = b;
		if(enterInterpretsSelection) {
			this.keyDownAction = { |view, char, mods, u, keycode, key|
				// 01000004 is Qt's keycode for Enter, needed on Mac
				if((char == Char.ret).or(key == 0x01000004)){
					case
					{ mods.isShift } {
						view.runJavaScript("selectLine()", this.onInterpret(_));
					}
					{ mods.isCtrl || mods.isCmd }{
						view.runJavaScript("selectRegion()", this.onInterpret(_));
					}
				}
			};
		}{
			this.keyDownAction = {}
		}
	}

	editable { ^this.getProperty('editable') }
	editable_ { |b| this.setProperty('editable', b) }

	pageBackgroundColor { ^this.getProperty('backgroundColor') }
	pageBackgroundColor_ { |color| this.setProperty('backgroundColor', color) }

	contentsSize { ^this.getProperty('contentsSize') }

	scrollPosition { ^this.getProperty('scrollPosition') }

	audioMuted { ^this.getProperty('audioMuted') }
	audioMuted_ { |muted| this.setProperty('audioMuted', muted) }

	overrideNavigation { ^this.getProperty('overrideNavigation') }
	overrideNavigation_ { |b| this.setProperty('overrideNavigation', b) }

	back { this.triggerPageAction(\back) }
	forward { this.triggerPageAction(\forward) }
	stop { this.triggerPageAction(\stop) }
	reload { this.triggerPageAction(\reload) }

}
