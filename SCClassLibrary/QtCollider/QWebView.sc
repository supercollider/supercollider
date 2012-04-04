QWebView : QView {

  var <onLoadFinished, <onLoadFailed, <onLinkActivated, <onReload, <onJavaScriptMsg;

  *qtClass { ^'QtCollider::WebView'; }

  *clearCache {
    _QWebView_ClearMemoryCaches
    ^this.primitiveFailed
  }

  url { ^this.getProperty( \url ); }

  url_ { arg address; this.setProperty( \url, address ); }

  title { ^this.getProperty( \title ); }

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

  evaluateJavaScript { arg script;
    this.invokeMethod( \evaluateJavaScript, script );
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

  // After this method is called with an object as an argument, QWebView will do
  // nothing when page reload is requested. Instead, the given object's 'value' method
  // will be called on such event.

  // The arguments passed to the 'value' method are this QWebView instance and
  // a String for the requested URL to be reloaded.

  // If this method is called with nil argument, QWebView's page reload handling will
  // be restored again.

  onReload_ { arg func;
    this.manageFunctionConnection( onReload, func, 'reloadTriggered(QString)' );
    this.setProperty( \delegateReload, func.notNil );
    onReload = func;
  }

  onJavaScriptMsg_ { arg func;
    this.manageFunctionConnection( onJavaScriptMsg, func,
      'jsConsoleMsg(const QString&, int, const QString&)' );
    onJavaScriptMsg = func;
  }

  enterInterpretsSelection { ^this.getProperty( \enterInterpretsSelection ); }

  enterInterpretsSelection_ { arg bool;
    this.setProperty( \enterInterpretsSelection, bool );
  }

  // Set a specific font family to be used in place of a CSS-specified generic font family.
  // The 'generic' argument must be one of the following symbols:
  // \standard, \fixed, \serif, \sansSerif, \cursive, \fantasy

  setFontFamily { arg generic, specific;
    this.invokeMethod( \setFontFamily, [QWebFontFamily(generic), specific] )
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
