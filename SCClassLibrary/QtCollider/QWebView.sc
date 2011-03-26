QWebView : QView {

  var <onLoadAction, <onLinkActivated;

  *qtClass { ^'QtCollider::WebView'; }

  url { ^this.getProperty( \url ); }

  url_ { arg address; this.setProperty( \url, address ); }

  // Get the displayed content in html form.
  html { ^this.getProperty( \html ); }

  // Set html content.
  html_ { arg htmlString;
    this.invokeMethod( \setHtml, htmlString );
  }

  selectedText { ^this.getProperty( \selectedText ); }

  // Try to extract plain text from html content and return it.
  plainText { ^this.getProperty( \plainText ); }

  reload { this.invokeMethod( 'reload' ); }

  back { this.invokeMethod( 'back' ); }

  forward { this.invokeMethod( 'forward' ); }

  findText { arg string, reverse = false;
    this.invokeMethod( \findText, [string, reverse] );
  }

  // The given function will be evaluated when a page has finished loading -
  // be it successfully or not.

  // The arguments passed to the function are the calling QWebView object,
  // and a boolean determining whether loading was successful.

  onLoad_ { arg func;
    this.manageFunctionConnection( onLoadAction, func, 'loadFinished(bool)' );
    onLoadAction = func;
  }

  // After this method is called with a function as an argument, QWebView will not
  // handle links in any way. Instead, the given function will be evaluated whenever
  // the user activates (clicks) a link.

  // The argument passed to the function is the calling QWebView object.

  // If this method is called with nil argument, QWebView link handling will be
  // restored again.

  onLinkActivated_ { arg func;
    this.manageFunctionConnection( onLinkActivated, func, 'linkActivated(QString)' );
    if (func.notNil)
      { this.setProperty( \linkDelegationPolicy, 2 ); }
      { this.setProperty( \linkDelegationPolicy, 0 ); };
    onLinkActivated = func;
  }
}

/*
perhaps also:
- history  // returning a string list of urls.
*/
