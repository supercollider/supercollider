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

	test_runJavaScript_returnTypes {
		(
			"1 + 1": 2 -> Integer,
			"1 + 0.1": 1.1 -> Float,
			"'hello'": "hello" -> String,
			"[1, 2, 'hello']": [1, 2, "hello"] -> Array
		).keysValuesDo { |jsCode, expected|
			var expectedClass = expected.value, expectedResult = expected.key;
			var condition, returnedResult;

			condition = false;
			WebView().runJavaScript(jsCode){ |res|
				returnedResult = res;
				condition = true;
			};

			this.wait({ condition }, "runJavaScript() callback was not called before timeout", 0.5);
			this.assertEquals(returnedResult, expectedResult,
				"runJavaScript(\"%\") should return the expected result %"
				.format(jsCode, expectedResult)
			);
			this.assert(returnedResult.class == expectedClass,
				"runJavaScript(\"%\") should return an instance of %. Returned: %"
				.format(jsCode, expectedClass, returnedResult.class)
			);
		};
	}
}
