QHelpBrowser : QView {

  *qtClass {^"QcHelpBrowser"}

  *new { arg parent, bounds;
    ^super.new(parent,bounds).initQHelpBrowser;
  }

  initQHelpBrowser {
    this.setSignalHandler( "searchRequest(const QString &, int)", \search );
    this.setSignalHandler( "classReferenceRequest(const QString &)", \showReference );
  }

  setCurrentIndex { arg index;
    this.invokeMethod( \setCurrentIndex, index );
  }

  setSearchResults { arg text, type, resultsData;
    this.invokeMethod( \setSearchResults, [text, type, resultsData] );
  }

  load { arg url;
    this.invokeMethod( \load, url );
  }

  search { arg text, type;
    var pool;
    var result;
    switch( type )
      { 0 } {
        result = Help.search( text );
        result = result.collect ({ |res|
          [res.docname, res.contextTrimmed, res.path];
        });
      }
      { 1 } {
        result = Class.allClasses.select({ |class|
          class.isMetaClass.not and: { class.name.asString.contains(text) }
        }).collect({ |class| class.name });
      }
      { 2 } {
        Class.allClasses.do({ |class|
          result = result.addAll(
            class.methods.select({ |method|
              method.name.asString.contains(text)
            }).collect({ |m| [class.name, m.name] })
          );
        });
      };

    this.setSearchResults( text, type, result );
  }

  showReference { arg class;
    class = class.asSymbol.asClass;
    if( class.notNil ) {
      this.invokeMethod( \setClassReference, [class.name, QHelpBrowser.classData( class )] );
    };
  }

  *classData { arg class;
    var name, superclass, subclasses, cvars, ivars, cmethods, imethods;
    name = class.name;
    superclass = class.superclass.name;
    subclasses = class.subclasses.collect({ |c| c.name });
    cvars = class.classVarNames;
    cmethods = class.class.methods.collect({ |m| [m.name].addAll(m.argNames) });
    ivars = class.instVarNames;
    imethods = class.methods.collect({ |m| [m.name].addAll(m.argNames) });

    ^[ name, superclass, subclasses, cvars, cmethods, ivars, imethods ];
  }
}
