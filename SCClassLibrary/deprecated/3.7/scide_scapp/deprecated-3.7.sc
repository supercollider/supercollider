+ CocoaDialog {
	*getPaths { arg okFunc, cancelFunc, allowsMultiple=true;
		this.deprecated(thisMethod, this.class.findMethod(\openPanel));

		if(result.notNil,{
			"A CocoaDialog is already in progress.  do: [CocoaDialog.clear]".warn;
			^nil
		});

		//result = Array.new(maxSize);
		ok = okFunc;
		cancel = cancelFunc;
		this.prGetPathsDialog(allowsMultiple);
	}
}
