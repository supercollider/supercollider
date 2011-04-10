HelpBrowser {
	classvar singleton;
	classvar <>defaultHomeUrl;

	var <>homeUrl;
	var <window;
	var webView;
	var lblStatus, animCount = 0;

	*instance {
		var homeUrl;
		if( singleton.isNil ) {
			homeUrl = defaultHomeUrl;
			if( homeUrl.isNil ) {
				homeUrl = SCDoc.helpTargetDir ++ "/Help.html";
			};
			singleton = this.new( homeUrl );
			singleton.window.onClose = { singleton = nil; };
		};
		^singleton;
	}

	*new { arg homeUrl;
		^super.new.init( homeUrl );
	}

	*openBrowser {
		this.instance.goTo(SCDoc.helpTargetDir++"/Browse.html").window.front;
	}
	*openSearch {|text|
		text = if(text.notNil) {"#"++text} {""};
		this.instance.goTo(SCDoc.helpTargetDir++"/Search.html"++text).window.front;
	}
	*openHelpFor {|text|
		this.instance.goTo(SCDoc.findHelpFile(text)).window.front;
	}

	goTo {|url|
		var newPath, oldPath, plainTextExts = #[".sc",".scd",".txt"];

		//FIXME: since multiple scdoc queries can be running at the same time,
		//it would be best to create a queue and run them in order, but only use the url from the last.
		#newPath, oldPath = [url,webView.url].collect {|x| if(x.notEmpty) {x.findRegexp("(^\\w+://)?([^#]+)(#.*)?")[1..].flop[1][1]}};

		plainTextExts.do {|x|
			if(newPath.endsWith(x)) {
				^this.openTextFile(newPath);
			}
		};

		if(newPath != oldPath) {
			this.startAnim;
		};

		Routine {
			webView.url = SCDoc.prepareHelpForURL(url, true) ?? {SCDoc.helpTargetDir++"/BrokenLink.html#"++url};
		}.play(AppClock);
	}

	goHome { this.goTo(homeUrl); }

	goBack { webView.back; }

	goForward { webView.forward; }

/* ------------------------------ private ------------------------------ */

	init { arg aHomeUrl;
		var toolbar;
		var lblFind, txtFind;
		var strh = "Tj".bounds.height;
		var vPad = 10, hPad = 20;
		var marg = 10;
		var winRect;
		var x, y, w, h;

		homeUrl = aHomeUrl;

		winRect = Rect(0,0,800,600);
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

		w = 100;
		x = x + 5;
		lblStatus = StaticText.new( window, Rect(x, y, w, h) )
			.resize_(1);
        lblStatus.font = Font(Font.defaultSansFace).boldVariant.size_(12);

		w = 200;
		x = winRect.width - marg - w;
		txtFind = TextField.new( window, Rect(x,y,w,h) ).resize_(3);

		w = "Find:".bounds.width + 5;
		x = x - w;
		lblFind = StaticText.new( window, Rect(x, y, w, h) )
			.string_("Find:")
			.resize_(3);

		x = 5;
		y = marg + h + 5;
		w = winRect.width - 10;
		h = winRect.height - y - marg;
		webView = WebView.new( window, Rect(x,y,w,h) ).resize_(5);

		webView.onLoadFinished = { this.stopAnim; window.name = "SuperCollider Help:"+webView.title };
		webView.onLoadFailed = { this.stopAnim };
		webView.onLinkActivated = {|wv, url|
			this.goTo(url);
		};

		toolbar[\Home].action = { this.goHome };
		toolbar[\Back].action = { this.goBack };
		toolbar[\Forward].action = { this.goForward };
		txtFind.action = { |x| webView.findText( x.string ); };
	}

	openTextFile {|path|
		var win, winRect, txt, file, fonts;
		if(Document.implementationClass.notNil) {
			^Document.open(path);
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
						    lblStatus.string_("Wait"+p);
						    0.3.wait;
						    if(animCount==0) {break.value};
					    };
				    };
			    };
			    lblStatus.string_("");
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
		HelpBrowser.instance.goHome.window.front;
	}
}
