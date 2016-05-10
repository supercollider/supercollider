+ Dialog {
	*getPaths { arg okFunc, cancelFunc, allowsMultiple=true;
		var fileMode;
		this.deprecated(thisMethod, this.class.findMethod(\openPanel));
		if( allowsMultiple ) { fileMode = 3 } { fileMode = 1 };
		^QFileDialog.new( okFunc, cancelFunc, fileMode, 0 );
	}
}
