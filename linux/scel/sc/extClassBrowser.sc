+ ClassBrowser{
	openSVN{ |svnAddr|
		Emacs.evalLispExpression(['w3m-browse-url', svnAddr].asLispString);
	}
}