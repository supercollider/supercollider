+ File {
	*openDialog { |prompt, successFunc, cancelFunc|
		this.deprecated(thisMethod, Dialog.findMethod(\openPanel));
		Dialog.openPanel(successFunc, cancelFunc)
	}

	*saveDialog { |prompt, defaultName, successFunc, cancelFunc|
		this.deprecated(thisMethod, Dialog.findMethod(\saveDialog));
		Dialog.savePanel(successFunc, cancelFunc)
	}
}
