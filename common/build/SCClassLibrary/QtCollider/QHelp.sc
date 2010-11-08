+ Help {

  gui { |sysext=true, userext=true, allowCached=true|
    var browser, listViews;
    var nodes;
    var fAddNode;
    var searchResults;

    // Call to ensure the tree has been built
    this.tree( sysext, userext, allowCached );

    browser = QHelpBrowser.new;
    listViews = Array.new;
    nodes = Array.new;

    fAddNode = { arg node;
      var newListView, newItems;
      var index;

      index = listViews.size;

      newListView = QListView.new;

      listViews = listViews.add( newListView );
      nodes = nodes.add( node );

      newItems =  node.keys(Array).collect(_.asString).sort({|a,b|
          // the outcomes:
          // a and b both start with open-bracket:
          //  test result should be a < b
          // or one starts with open-bracket and the other doesn't (xor)
          //  test result should be whether it's a that has the bracket
        if(a[0] == $[ /*]*/ xor: (b[0] == $[ /*]*/)) {
          a[0] == $[ /*]*/
        } {
          a < b
        }
      });
      newListView.items = newItems;

      newListView.action = { arg list;
        var activatedNode, activatedItem;

        listViews[(index+1)..].do {|x| x.remove;};
        listViews.removeAll( listViews[(index+1)..] );
        nodes.removeAll( nodes[index+1..] );

        activatedItem = list.item;
        activatedNode = try { node[activatedItem] };
        if( activatedNode.notNil ) {
          fAddNode.value( activatedNode );
          browser.setCurrentIndex( index+1 );
        } {
          browser.load( fileslist.at( activatedItem.asSymbol ) ? fileslist.at( \Help ) );
        }
      };

      newListView.setParent( browser );
    };

    browser.fSearch = { arg me, searchString;
      var resultsData;
      searchResults = this.search( searchString );
      resultsData = searchResults.collect ({ |res|
        [res.docname, res.contextTrimmed];
      });
      browser.setSearchResults( resultsData );
    };

    browser.fSearchResultActivated = { arg me, index;
      var res;
      res = searchResults[index];
      if( res.notNil ) {
        browser.load( res.path );
      }
    };

    fAddNode.value( tree );

    browser.front;
    browser.load( fileslist.at( \Help ) );

    ^browser;
  }

}
