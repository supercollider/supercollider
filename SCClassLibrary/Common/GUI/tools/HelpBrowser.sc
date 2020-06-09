HelpBrowser {
	classvar singleton;
	classvar <>defaultHomeUrl;
	classvar <>openNewWindows = false;

	var <>homeUrl;
	var <window;
	var webView;
	var loading = false;
	var srchBox;
	var openNewWin;
	var rout;

	*initClass {
		Class.initClassTree(SCDoc);
		defaultHomeUrl = SCDoc.helpTargetUrl ++ "/Help.html";

		// Skip the rest of the init on Qt-less builds; the WebView
		// class has dependencies on Qt primitives.
		if(Platform.hasQt.not) { ^nil; };
		StartUp.add {
			NotificationCenter.register(SCDoc, \didIndexAllDocs, this) { WebView.clearCache }
		}
	}

	*instance {
		if( singleton.isNil ) {
			singleton = this.new;
		};
		^singleton;
	}

	*new { arg aHomeUrl, newWin;
		^super.new.init( aHomeUrl ? defaultHomeUrl, newWin ?? { openNewWindows } );
	}

	*goTo {|url|
		var ideClass = \ScIDE.asClass;
		if ( ideClass.notNil ) {
			ideClass.openHelpUrl(url);
		}{
			this.front.goTo(url);
		}
	}

	*front {
		var w = if(openNewWindows,{this.new},{this.instance});
		w.window.front;
		^w;
	}

	*goHome { this.goTo(defaultHomeUrl); }

	*openBrowsePage {|category|
		category = if(category.notNil) {"#"++category} {""};
		this.goTo(SCDoc.helpTargetUrl++"/Browse.html"++category);
	}
	*openSearchPage {|text|
		text = if(text.notNil) {"#"++text} {""};
		this.goTo(SCDoc.helpTargetUrl++"/Search.html"++text);
	}
	*openHelpFor {|text|
		this.goTo(SCDoc.findHelpFile(text));
		/* The following was replaced, for compatibility with SC IDE:
		var w = this.front;
		{ w.startAnim; w.goTo(SCDoc.findHelpFile(text)) }.fork(AppClock);
		*/
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
		this.goTo(SCDoc.helpTargetUrl++"/Classes/"++cls++".html#"++met);
	}

	cmdPeriod { rout.play(AppClock) }
	goTo {|urlString, brokenAction|
		var url, newPath, oldPath;

		window.front;
		this.startAnim;

		brokenAction = brokenAction ? { |fragment|
			var brokenUrl = URI.fromLocalPath( SCDoc.helpTargetDir++"/BrokenLink.html" );
			brokenUrl.fragment = fragment;
			brokenUrl;
		};

		url = URI(urlString);

		rout = Routine {
			try {
				url = SCDoc.prepareHelpForURL(url) ?? { brokenAction.(urlString) };
				newPath = url.path;
				oldPath = URI(webView.url).path;
				webView.url = url.asString;
				// needed since onLoadFinished is not called if the path did not change:
				if(newPath == oldPath) {webView.onLoadFinished.value};
				webView.focus;
			} {|err|
				webView.setHtml(err.errorString);
				err.throw;
			};
			CmdPeriod.remove(this);
			rout = nil;
		}.play(AppClock);
		CmdPeriod.add(this);
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

		window.onClose = {
			this.stopAnim;
			if(singleton == this) {
				singleton = nil;
			};
		};

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
					{SCDoc.helpTargetUrl ++ "/Search.html" ++ x.string}
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
				.action_({ |b| openNewWin = b.value; webView.overrideNavigation = openNewWin; });
		} {
			str = "Open links in same window";
			w = str.bounds.width + 5;
			Button.new( window, Rect(x, y, w, h) )
				.resize_(1)
				.states_([[str],["Open links in new window"]])
				.value_(openNewWin.asInteger)
				.action_({ |b| openNewWin = b.value.asBoolean; webView.overrideNavigation = openNewWin; });
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
		webView.setHtml("Please wait while initializing Help... (This might take several seconds the first time)");

		webView.setFontFamily(\fixed, Platform.case(
			\osx, { "Monaco" },
			\linux, { "Andale Mono" },
			{ "Monospace" }
		));

		webView.onLoadFinished = {
			this.stopAnim;
			window.name = "SuperCollider Help: %".format(webView.title);
		};

		webView.onLoadFailed = { this.stopAnim };

		webView.onLinkActivated = {|wv, url|
			var redirected, newPath, oldPath;
			redirected = this.redirectTextFile(url);

			if (not(redirected)) {
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
			}
		};

		webView.onReloadTriggered = {|wv, url|
			WebView.clearCache;
			this.goTo(url);
		};

		webView.onJavaScriptMsg = {|wv, err, type|
			"JavaScript %: %".format(if(type==0,"Error","Message"),err).postln;
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
		window.view.keyDownAction = { arg view, char, mods, uni, kcode, key;
			var keyPlus, keyZero, keyMinus, keyEquals, keyF;
			var modifier, zoomIn;

			modifier = Platform.case(\osx, {
				mods.isCmd && mods.isCtrl.not && mods.isAlt.not;
			}, {
				mods.isCtrl && mods.isCmd.not && mods.isAlt.not;
			});

			#keyPlus, keyZero, keyMinus, keyEquals, keyF = [43, 48, 45, 61, 70];

			// +/= has the same value on macOS when pressed with <Cmd>
			zoomIn = Platform.case(\osx, {
				(key == keyEquals) && modifier;
			}, {
				(key == keyPlus) && modifier;
			});

			if (zoomIn) {
				webView.zoom = min(webView.zoom + 0.1, 2.0);
			};

			if ((key == keyMinus) && modifier) {
				webView.zoom = max(webView.zoom - 0.1, 0.1);
			};

			if ((key == keyZero) && modifier) {
				webView.zoom = 1.0;
			};

			if ((key == keyF) && modifier) {
				toggleFind.value;
			};

			if (char.ascii == 27) { // Esc
				if (findView.visible) { toggleFind.value };
			};
		};

		toolbar[\Back].action = { this.goBack };
		toolbar[\Forward].action = { this.goForward };
		toolbar[\Reload].action = { this.goTo( webView.url ) };
		txtFind.action = { |x| webView.findText( x.string ) };

		{ this.goHome() }.defer;
	}

	redirectTextFile {|url|
		var plainTextExts = #[".sc",".scd",".txt",".schelp",".rtf"];

		plainTextExts.do {|x|
			var path;
			if(url.endsWith(x)) {
				path = url.replace("%20"," ")
					.findRegexp("(^\\w+://)?([^#]+)(#.*)?")[1..].flop[1][1];
				if(File.exists(path)) {
					path.openDocument;
				} {
					webView.url = SCDoc.helpTargetUrl++"/BrokenLink.html#"++path;
					window.front;
				};
				^true
			}
		};

		^false
	}

	startAnim {
		var progress = [">---","->--","-->-","--->"];
		if(loading.not) {
			loading = true;
			Routine {
				block {|break|
					loop {
						progress.do {|p|
							window.name = ("Loading"+p);
							0.3.wait;
							if(loading.not) {break.value};
						};
					};
				};
//				lblStatus.string_("");
			}.play(AppClock);
		};
	}
	stopAnim {
		loading = false;
	}

}
