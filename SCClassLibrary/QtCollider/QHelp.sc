+ Help {

  gui { |sysext=true, userext=true, allowCached=true|
    var browser = QHelpBrowser.instance;
    if( browser.isNil ) {
      browser = QHelpBrowser.newUnique;
      browser.home = fileslist.at( \Help );
    }
    ^browser.front;
  }

}
