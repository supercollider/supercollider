FileDialog : QObject {
	*qtClass { ^'QcFileDialog' }

	*new { arg okFunc, cancelFunc, fileMode, acceptMode, stripResult = false;
		/**
		fileMode:
			QFileDialog::AnyFile		0	The name of a file, whether it exists or not.
			QFileDialog::ExistingFile	1	The name of a single existing file.
			QFileDialog::Directory		2	The name of a directory. Both files and directories are displayed.
			QFileDialog::ExistingFiles	3	The names of zero or more existing files.
		acceptMode:
			QFileDialog::AcceptOpen		0
			QFileDialog::AcceptSave		1

		stripResult:
			true: okFunc(path1, path2, path3)
			false: okFunc(paths)
		**/

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
