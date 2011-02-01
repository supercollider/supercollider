+ Help {

  gui { |sysext=true, userext=true, allowCached=true|
    var browser, bounds;

    bounds = Rect(0,0,800,600);
    bounds = bounds.moveToPoint( bounds.centerIn( QWindow.screenBounds ) );

    browser = QHelpBrowser.new( nil, bounds );
    browser.load( fileslist.at( \Help ) );
    browser.front;

    ^browser;
  }

}
