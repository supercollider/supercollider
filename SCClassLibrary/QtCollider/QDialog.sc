QFileDialog : QObject {
  *new { arg okFunc, cancelFunc, fileMode, acceptMode;
    var me = super.new("QcFileDialog", [fileMode, acceptMode] );
    if( okFunc.notNil ) {
      me.connectFunction( 'accepted(VariantList)', { |me, result| okFunc.value(result) } );
    };
    if( cancelFunc.notNil ) {
      me.connectFunction( 'rejected()', { cancelFunc.value() } );
    };
    me.invokeMethod('show');
    ^me;
  }
}

QDialog {
  *getPaths { arg okFunc, cancelFunc, allowsMultiple=true;
    var fileMode;
    if( allowsMultiple ) { fileMode = 3 } { fileMode = 1 };
    ^QFileDialog.new( okFunc, cancelFunc, fileMode, 0 );
  }

  *savePanel { arg okFunc, cancelFunc;
    ^QFileDialog.new( okFunc, cancelFunc, 0, 1 );
  }
}
