+ QWebView {

	// Get the displayed content in html form.
	html {
		this.deprecated(thisMethod, this.class.findRespondingMethodFor(\toHtml));
		^"";
	}

	// Set html content.
	html_{
		|htmlString|
		this.deprecated(thisMethod, this.class.findRespondingMethodFor(\setHtml));
		^this.setHtml(htmlString)
	}

	// Try to extract plain text from html content and return it.
	plainText {
		this.deprecated(thisMethod, this.class.findRespondingMethodFor(\toPlainText));
		^"";
	}

	evaluateJavaScript {
		|script|
		this.deprecated(thisMethod, this.class.findRespondingMethodFor(\runJavaScript));
		^this.invokeMethod(\runJavaScript, script);
	}

	onReload_ {
		|func|
		this.deprecated(thisMethod, this.class.findRespondingMethodFor(\onReloadTriggered));
		^this.invokeMethod(\onReloadTriggered, func);
	}
}
