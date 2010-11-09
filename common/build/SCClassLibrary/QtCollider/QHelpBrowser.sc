QHelpBrowser : QView {
  var >fSearch;
  var >fSearchResultActivated;

  *qtClass {^"QcHelpBrowser"}

  *new { arg parent, bounds;
    ^super.new(parent,bounds).initQHelpBrowser;
  }

  initQHelpBrowser {
    this.setSignalHandler( "searchInvoked(const QString &)", \search );
    this.setSignalHandler( "searchResultActivated(int)", \searchResultActivated );
  }

  setCurrentIndex { arg index;
    this.invokeMethod( \setCurrentIndex, index );
  }

  setSearchResults { arg resultsData;
    this.invokeMethod( \setSearchResults, [resultsData] );
  }

  load { arg url;
    this.invokeMethod( \load, url );
  }

  search { arg text;
    fSearch.value( this, text );
  }

  searchResultActivated { arg index;
    fSearchResultActivated.value( this, index );
  }
}
