FileDialog : QObject {
	*qtClass { ^'QcFileDialog' }

	*new { arg okFunc, cancelFunc, fileMode, acceptMode, stripResult = false;

		var me = super.new( [fileMode, acceptMode] );

		if( okFunc.notNil ) {
			me.connectFunction( 'accepted(QVariantList)', {
				|me, result|
				if( stripResult )
				{ okFunc.performList(\value, result) }
				{ okFunc.value(result) }
			});
		};

		if( cancelFunc.notNil ) {
			me.connectFunction( 'rejected()', { cancelFunc.value() } );
		};

		me.invokeMethod('show', synchronous:false);

		^me;
	}
}

Dialog {

	*openPanel { arg okFunc, cancelFunc, multipleSelection=false;
		var fileMode;
		if( multipleSelection ) { fileMode = 3 } { fileMode = 1 };
		^FileDialog.new( okFunc, cancelFunc, fileMode, 0, stripResult:multipleSelection.not );
	}

	*savePanel { arg okFunc, cancelFunc;
		^FileDialog.new( okFunc, cancelFunc, 0, 1, stripResult:true );
	}
}
