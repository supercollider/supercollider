HelpBrowser {
	classvar singleton;
	classvar <>defaultHomeUrl;
	classvar <>openNewWindows = false;

	var <>homeUrl;
	var <window;
	var webView;
	var animCount = 0;
	var txtPath;
	var openNewWin;

	*initClass {
		StartUp.add {
			NotificationCenter.register(SCDoc, \docMapDidUpdate, this) {
				if(WebView.implClass.respondsTo(\clearCache)) {
					WebView.clearCache;
				}
			}
		}
	}

	*instance {
		if( singleton.isNil ) {
			singleton = this.new;
			singleton.window.onClose = { singleton = nil; };
		};
		^singleton;
	}

	*new { arg aHomeUrl, newWin;
		if( aHomeUrl.isNil ) {
			aHomeUrl = defaultHomeUrl ?? { SCDoc.helpTargetDir ++ "/Help.html" };
		};
		^super.new.init( aHomeUrl, newWin ?? { openNewWindows } );
	}

	*goTo {|url|
		if(openNewWindows,{this.new},{this.instance}).goTo(url);
	}

	*openBrowser {
		this.goTo(SCDoc.helpTargetDir++"/Browse.html");
	}
	*openSearch {|text|
		text = if(text.notNil) {"#"++text} {""};
		this.goTo(SCDoc.helpTargetDir++"/Search.html"++text);
	}
	*openHelpFor {|text|
		this.goTo(SCDoc.findHelpFile(text));
	}

	goTo {|url, brokenAction|
		var newPath, oldPath, plainTextExts = #[".sc",".scd",".txt",".schelp"];

		//FIXME: since multiple scdoc queries can be running at the same time,
		//it would be best to create a queue and run them in order, but only use the url from the last.

		plainTextExts.do {|x|
			if(url.endsWith(x)) {
				^this.openTextFile(url);
			}
		};

		this.startAnim;

		brokenAction = brokenAction ? {SCDoc.helpTargetDir++"/BrokenLink.html#"++url};
		Routine {
			try {
				url = SCDoc.prepareHelpForURL(url) ?? brokenAction;
				#newPath, oldPath = [url,webView.url].collect {|x|
					if(x.notEmpty) {x.findRegexp("(^\\w+://)?([^#]+)(#.*)?")[1..].flop[1][1]}
				};
				webView.url = url;
				// needed since onLoadFinished is not called if the path did not change:
				if(newPath == oldPath) {webView.onLoadFinished.value};
			} {|err|
				webView.html = err.errorString;
				err.throw;
			};
		}.play(AppClock);
		window.front;
	}

	goHome { this.goTo(homeUrl); }

	goBack { webView.back; }

	goForward { webView.forward; }

/* ------------------------------ private ------------------------------ */

	init { arg aHomeUrl, aNewWin;
		var toolbar;
		var lblFind, txtFind, findView, saveSize, toggleFind;
		var strh = "Tj".bounds.height;
		var vPad = 10, hPad = 20;
		var marg = 10;
		var winRect;
		var x, y, w, h;
		var str;

		homeUrl = aHomeUrl;

		winRect = Rect(0, 0, 800, (Window.screenBounds.height * 0.8).floor);
		winRect = winRect.moveToPoint(winRect.centerIn(Window.screenBounds));

		window = Window.new( bounds: winRect ).name_("SuperCollider Help");

		toolbar = ();

		h = strh + vPad;
		x = marg; y = marg;
		[\Home, \Back, \Forward].do { |sym|
			var str = sym.asString;
			var w = str.bounds.width + hPad;
			toolbar[sym] = Button( window, Rect(x,y,w,h) ).states_([[str]]);
			x = x + w + 2;
		};

		str = "Path:";
		w = str.bounds.width + 5;
		x = x + 5;
		StaticText.new( window, Rect(x, y, w, h) )
			.string_(str)
			.resize_(1);
		x = x + w;

		w = 200;
		txtPath = TextField.new( window, Rect(x,y,w,h) ).resize_(1);
		txtPath.action = {|x|
			var path, hash, fallback;
			if(x.string.notEmpty) {
				#path, hash = x.string.findRegexp("([^#]+)(#?.*)")[1..].flop[1];
				fallback = {SCDoc.helpTargetDir++"/Search.html#"++x.string};
				if(hash.isEmpty) {
					this.goTo(SCDoc.helpTargetDir +/+ path ++ ".html", fallback)
				} {
					this.goTo(SCDoc.helpTargetDir +/+ path ++ ".html" ++ hash, fallback)
				}
			}
		};

		openNewWin = aNewWin;
		x = x + w + 10;
		if(GUI.scheme==QtGUI) {
			str = "Open links in new window";
			w = str.bounds.width + 50;
			QCheckBox.new (window, Rect(x, y, w, h) )
				.resize_(1)
				.string_(str)
				.value_(openNewWin)
				.action_({ |b| openNewWin = b.value });
		} {
			str = "Open links in same window";
			w = str.bounds.width + 5;
			Button.new( window, Rect(x, y, w, h) )
				.resize_(1)
				.states_([[str],["Open links in new window"]])
				.value_(openNewWin.asInteger)
				.action_({ |b| openNewWin = b.value.asBoolean });
		};

		x = 0;
		y = marg + h + 5;
		w = winRect.width;
		findView = CompositeView(window, Rect(x,y,w,h+10)).visible_(false).resize_(2);
		y = marg;
		w = 200;
		x = winRect.width - marg - w;
		txtFind = TextField.new( findView, Rect(x,y,w,h) ).resize_(3);
		str = "Find text in document:";
		w = str.bounds.width + 5;
		x = x - w - 5;
		lblFind = StaticText.new( findView, Rect(x, y, w, h) )
			.string_(str)
			.resize_(3);

		x = 5;
		y = marg + h + 5;
		w = winRect.width - 10;
		h = winRect.height - y - marg;
		webView = WebView.new( window, Rect(x,y,w,h) ).resize_(5);
		webView.html = "Please wait while initializing Help... (This might take several seconds the first time)";

		webView.onLoadFinished = {
			this.stopAnim;
			window.name = "SuperCollider Help:"+webView.title;
			if(webView.url.beginsWith("file://"++SCDoc.helpTargetDir)) {
				txtPath.string = webView.url.findRegexp("file://"++SCDoc.helpTargetDir++"/([^#]+?)\\.[^\\.]+$")[1][1];
			} {
				txtPath.string = webView.url;
			}
		};
		webView.onLoadFailed = { this.stopAnim };
		webView.onLinkActivated = {|wv, url|
			var newPath, oldPath;
			if(openNewWin) {
				#newPath, oldPath = [url,webView.url].collect {|x|
					if(x.notEmpty) {x.findRegexp("(^\\w+://)?([^#]+)(#.*)?")[1..].flop[1][1]}
				};
			};
			if(newPath!=oldPath) {
				HelpBrowser.new(newWin:true).goTo(url);
			} {
				this.goTo(url);
			};
		};
		if(webView.respondsTo(\onReload_)) {
			webView.onReload = {|wv, url|
				this.goTo(url);
			};
		};

		toggleFind = {
				if(findView.visible.not) {
					saveSize = webView.bounds;
					h = findView.bounds.height + marg;
					webView.bounds = Rect(saveSize.left,saveSize.top+h,saveSize.width,saveSize.height-h);
					findView.visible = true;
					txtFind.focus;
				} {
					webView.bounds = saveSize;
					findView.visible = false;
				};
		};

		webView.enterInterpretsSelection = true;
		webView.keyDownAction = { arg view, char, mods;
			if( (char.ascii == 13) && (mods.isCtrl || mods.isCmd || mods.isShift) ) {
				view.tryPerform(\evaluateJavaScript,"selectLine()");
			};
		};
		window.view.keyDownAction = { arg view, char, mods;
			if( ((char.ascii == 6) && mods.isCtrl) || (char == $f && mods.isCmd) ) {
				toggleFind.value;
			};
			if(char.ascii==27) {
				if(findView.visible) {toggleFind.value};
			}
		};

		toolbar[\Home].action = { this.goHome };
		toolbar[\Back].action = { this.goBack };
		toolbar[\Forward].action = { this.goForward };
		txtFind.action = { |x| webView.findText( x.string ); };
	}

	openTextFile {|path|
		var win, winRect, txt, file, fonts;
		path = path.replace("%20"," ").findRegexp("(^\\w+://)?([^#]+)(#.*)?")[1..].flop[1][1];
		if(Document.implementationClass.notNil) {
			^Document.open(path);
		};
		if("which xdg-open >/dev/null".systemCmd==0) {
			^("xdg-open"+path.escapeChar($ )).systemCmd;
		};
		winRect = Rect(0,0,600,400);
		winRect = winRect.moveToPoint(winRect.centerIn(Window.screenBounds));
		file = File(path,"r");
		win = Window(bounds: winRect).name_("SuperCollider Help:"+path.basename);
		txt = TextView(win,Rect(0,0,600,400)).resize_(5).string_(file.readAllString);
		file.close;
		fonts = Font.availableFonts;
		txt.font = Font(["Monaco","Monospace"].detect {|x| fonts.includesEqual(x)} ?? {Font.defaultMonoFace}, 12);
		win.front;
	}

	startAnim {
		var progress = [">---","->--","-->-","--->"];
		animCount = animCount + 1;
		if(animCount==1) {
			Routine {
				block {|break|
					loop {
						progress.do {|p|
							window.name = ("Loading"+p);
							0.3.wait;
							if(animCount==0) {break.value};
						};
					};
				};
//				lblStatus.string_("");
			}.play(AppClock);
		};
	}
	stopAnim {
		if(animCount>0) {
			animCount = animCount - 1;
		};
	}

}

+ Help {
	gui {
		HelpBrowser.instance.goHome;
	}
}
