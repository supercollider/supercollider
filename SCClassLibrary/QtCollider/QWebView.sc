QWebView : QView {

  var <onLoadFinished, <onLoadFailed, <onLinkActivated;

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

  // The given function will be evaluated when a page has loaded successfully.
  // The calling QWebView object is passed to the function.

  onLoadFinished_ { arg func;
    case
    { (func ? onLoadFailed).notNil && (onLoadFinished ? onLoadFailed).isNil }
        { this.connectMethod( 'loadFinished(bool)', 'prLoadFinished' ); }
    { (func ? onLoadFailed).isNil && (onLoadFinished ? onLoadFailed).notNil }
        { this.disconnectMethod( 'loadFinished(bool)', 'prLoadFinished' ); };

    onLoadFinished = func;
  }

  // The given function will be evaluated when a page has failed to load.
  // The calling QWebView object is passed to the function.

  onLoadFailed_ { arg func;
    case
    { (func ? onLoadFinished).notNil && (onLoadFinished ? onLoadFailed).isNil }
        { this.connectMethod( 'loadFinished(bool)', 'prLoadFinished' ); }
    { (func ? onLoadFinished).isNil && (onLoadFinished ? onLoadFailed).notNil }
        { this.disconnectMethod( 'loadFinished(bool)', 'prLoadFinished' ); };

    onLoadFailed = func;
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

//---------------------------------- private --------------------------------------//

  prLoadFinished { arg ok;
    if(ok) { onLoadFinished.value(this) } { onLoadFailed.value(this) };
  }
}

/*
perhaps also:
- history  // returning a string list of urls.
*/
