+ File {
	*openDialog { arg prompt, successFunc, cancelFunc;
		Dialog.openPanel(successFunc, cancelFunc);
	}

	*saveDialog { arg prompt, defaultName, successFunc, cancelFunc;
		Dialog.savePanel(successFunc, cancelFunc);
	}
}
