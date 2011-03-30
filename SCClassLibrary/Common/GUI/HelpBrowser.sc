HelpBrowser {
	classvar singleton;
	classvar <>defaultHomeUrl;

	var <>homeUrl;
	var <window;
	var webView;
	var lblStatus;

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

	goTo {|url|
		var done = false, progress = [">---","->--","-->-","--->"];
		var r = Routine {
			block {|break|
				loop {
					progress.do {|p|
						lblStatus.string_("Wait"+p);
						0.2.wait;
						if(done) {break.value};
					};
				};
			};
			lblStatus.string_("");
		}.play(AppClock,0);
		
		Routine {
			webView.url = SCDoc.prepareHelpForURL(url, true);
			done = true;
		}.play(AppClock,0);
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

		winRect = Rect(0,0,600,600);
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

		webView.onLinkActivated = {|wv, url|
			this.goTo(url);
		};

		toolbar[\Home].action = { this.goHome };
		toolbar[\Back].action = { this.goBack };
		toolbar[\Forward].action = { this.goForward };
		txtFind.action = { |x| webView.findText( x.string ); };

		window.front;
		this.goHome;
	}
}

+ Help {
	gui {
		HelpBrowser.instance.window.front;
	}
}
