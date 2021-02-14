TestWebView : UnitTest {

	test_findText_found {
		var window = Window();
		var webView = WebView(window, Rect(0, 0, 100, 100));
		var cond = Condition.new;
		var wasFound, timeout;

		window.front; // FIXME findText doesn't work if the window isn't displayed
		webView.setHtml("abc");
		webView.onLoadFinished = {
			webView.findText("abc", true, { |found|
				cond.unhang;
				wasFound = found;
			});
		};
		timeout = fork {
			2.wait;
			cond.unhang;
		};

		cond.hang;
		timeout.stop;
		window.close;
		this.assert(wasFound.notNil, "test timeout check");
		if(wasFound.notNil) {
			this.assert(wasFound, "findText should find")
		}
	}

	test_findText_notFound {
		var window = Window();
		var webView = WebView(window, Rect(0, 0, 100, 100));
		var cond = Condition.new;
		var wasFound, timeout;

		window.front; // FIXME findText doesn't work if the window isn't displayed
		webView.setHtml("abc");
		webView.onLoadFinished = {
			webView.findText("def", true, { |found|
				cond.unhang;
				wasFound = found;
			});
		};
		timeout = fork {
			2.wait;
			cond.unhang;
		};

		cond.hang;
		timeout.stop;
		window.close;
		this.assert(wasFound.notNil, "test timeout check");
		if(wasFound.notNil) {
			this.assert(wasFound.not, "findText should not find");
		}
	}
}
