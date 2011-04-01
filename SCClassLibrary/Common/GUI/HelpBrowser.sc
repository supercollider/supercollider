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

	goTo {|url|
   	//FIXME: since multiple scdoc queries can be running at the same time,
	//it would be best to create a queue and run them in order, but only use the url from the last.
		this.startAnim;
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
        lblStatus.font = Font.defaultSansFace.boldVariant;

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

		webView.onLoadFinished = { this.stopAnim };
		webView.onLoadFailed = { this.stopAnim };
		webView.onLinkActivated = {|wv, url|
			this.goTo(url);
		};

		toolbar[\Home].action = { this.goHome };
		toolbar[\Back].action = { this.goBack };
		toolbar[\Forward].action = { this.goForward };
		txtFind.action = { |x| webView.findText( x.string ); };
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
