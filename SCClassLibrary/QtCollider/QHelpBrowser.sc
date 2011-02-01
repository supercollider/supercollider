QHelpBrowser : QView {

  *qtClass {^"QcHelpBrowser"}

  *new { arg parent, bounds;
    ^super.new(parent,bounds).initQHelpBrowser;
  }

  initQHelpBrowser {
  }

  load { arg url;
    this.invokeMethod( \load, url );
  }
}
