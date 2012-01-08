HelpBrowser {
	classvar singleton;
	classvar <>defaultHomeUrl;
	classvar <>openNewWindows = false;

	var <>homeUrl;
	var <window;
	var webView;
	var animCount = 0;
	var srchBox;
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
			singleton.window.onClose = {
				singleton.stopAnim;
				singleton = nil;
			};
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

	*openBrowsePage {|category|
		category = if(category.notNil) {"#"++category} {""};
		this.goTo(SCDoc.helpTargetDir++"/Browse.html"++category);
	}
	*openSearchPage {|text|
		text = if(text.notNil) {"#"++text} {""};
		this.goTo(SCDoc.helpTargetDir++"/Search.html"++text);
	}
	*openHelpFor {|text|
		this.goTo(SCDoc.findHelpFile(text));
	}
	*openHelpForMethod {|method|
		var cls = method.ownerClass;
		var met = method.name.asString;
		if(cls.isMetaClass) {
			cls = cls.name.asString.drop(5);
			met = "*"++met;
		} {
			cls = cls.name.asString;
			met = "-"++met;
		};
		this.goTo(Help.dir+/+"Classes"+/+cls++".html#"++met);
	}
	*getOldWrapUrl {|url|
		var c;
		^("file://" ++ SCDoc.helpTargetDir +/+ "OldHelpWrapper.html#"++url++"?"++
		SCDoc.helpTargetDir +/+ if((c=url.basename.split($.).first).asSymbol.asClass.notNil)
			{"Classes" +/+ c ++ ".html"}
			{"Guides/WritingHelp.html"})
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

		window.front;
		this.startAnim;

		brokenAction = brokenAction ? {SCDoc.helpTargetDir++"/BrokenLink.html#"++url};
		Routine {
			try {
				url = SCDoc.prepareHelpForURL(url) ?? brokenAction;
				#newPath, oldPath = [url,webView.url].collect {|x|
					if(x.notEmpty) {x.findRegexp("(^\\w+://)?([^#]+)(#.*)?")[1..].flop[1][1]}
				};
				// detect old helpfiles and open them in OldHelpWrapper
				if(block{|break| Help.do {|key,path| if(url.endsWith(path)) {break.value(true)}}; false}) {
					url = HelpBrowser.getOldWrapUrl(url)
				};
				webView.url = url;
				// needed since onLoadFinished is not called if the path did not change:
				if(newPath == oldPath) {webView.onLoadFinished.value};
				webView.focus;
			} {|err|
				webView.html = err.errorString;
				err.throw;
			};
		}.play(AppClock);
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
		[[\Back,"<"], [\Forward,">"], [\Reload, "Reload"]].do { |item|
			var str = item[1];
			var w = str.bounds.width + hPad;
			toolbar[item[0]] = Button( window, Rect(x,y,w,h) ).states_([[str]]);
			x = x + w + 2;
		};

		x = x + 10;
		str = "Quick lookup:";
		w = str.bounds.width + 5;
		StaticText(window, Rect(x,y,w,h)).string_(str);
		x = x + w;
		w = 200;
		srchBox = TextField.new( window, Rect(x,y,w,h) ).resize_(1);
		if(GUI.current.id == \qt) {
			srchBox.toolTip = "Smart quick help lookup. Prefix with # to just search.";
		};
		srchBox.action = {|x|
			if(x.string.notEmpty) {
				this.goTo(if(x.string.first==$#)
					{SCDoc.helpTargetDir++"/Search.html#"++x.string.drop(1)}
					{SCDoc.findHelpFile(x.string)}
				);
			}
		};

		openNewWin = aNewWin;
		x = x + w + 10;
		if(GUI.current.respondsTo(\checkBox)) {
			str = "Open links in new window";
			w = str.bounds.width + 50;
			CheckBox.new (window, Rect(x, y, w, h) )
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

		if(webView.respondsTo(\setFontFamily)) {
			webView.setFontFamily(\fixed, Platform.case(
				\osx, { "Monaco" },
				\linux, { "Andale Mono" },
				{ "Monospace" }
			))
		};

		webView.onLoadFinished = {
			this.stopAnim;
			window.name = "SuperCollider Help: %".format(webView.title);
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

		toolbar[\Back].action = { this.goBack };
		toolbar[\Forward].action = { this.goForward };
		toolbar[\Reload].action = { this.goTo( webView.url ) };
		txtFind.action = { |x| webView.findText( x.string ); };
	}

	openTextFile {|path|
		var win, winRect, txt, file, fonts;
		path = path.replace("%20"," ").findRegexp("(^\\w+://)?([^#]+)(#.*)?")[1..].flop[1][1];
		if(File.exists(path)) {
			path.openDocument;
		} {
			webView.url = SCDoc.helpTargetDir++"/BrokenLink.html#"++path;
			window.front;
		}
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
	*gui {
		HelpBrowser.instance.goHome;
	}
}
