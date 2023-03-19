+ File {
	*openDialog { arg prompt, successFunc, cancelFunc;
		this.deprecated(thisMethod, Dialog.findMethod(\openPanel));
		Dialog.openPanel(successFunc, cancelFunc);
	}

	*saveDialog { arg prompt, defaultName, successFunc, cancelFunc;
		this.deprecated(thisMethod, Dialog.findMethod(\saveDialog));
		Dialog.savePanel(successFunc, cancelFunc);
	}
}
