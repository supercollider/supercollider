+ Helper{

*updateRedirectStubs { |wpath, relativeLinks=true|
	var template, tfile, classesArray, path;
	path= Platform.helpDir+/+"/GUI/Main-GUI/";
	wpath = wpath ? Platform.helpDir.asRelativePath;
	wpath= wpath +/+"/GUI/Main-GUI/";

	tfile=File(path+/+"StubTemplate.html","r");  // read the html template file
	template=tfile.readAllString;
	tfile.close;

	classesArray = classesArray ?  ViewRedirect.allSubclasses;

	classesArray.do {arg class;
		var file, name, string, links="";

		(wpath+/+class.name.asString++".html").postln;

		file=File(wpath+/+class.name.asString++".html", "w"); //create the stub file

		GUI.schemes.do{arg scheme;  // for each registerd scheme
			var nm, helpFilePath="";
			try{  // if this fails, then there is no gui class in the kit for this redirect.

				nm=scheme.perform(class.key).name.asString;  //Get the kit-specific class name

				{
					helpFilePath=nm.findHelpFile;
					if ( relativeLinks.not, {
						helpFilePath = "file://" ++ helpFilePath;
					},{
						helpFilePath = helpFilePath.asRelativePath(Platform.helpDir+/+"/GUI/Main-GUI");

					});
				}.try({helpFilePath=nm++".html"});

				//if you can't find the helpfile path, then just create a file name

				links=links++(("<p class=\"p1\"><b>%:<span class=\"Apple-tab-span\">     </span>"
					 ++"</b> <a href=\"%\">%</a></p>\n<br>\n")
					.format(scheme ,helpFilePath,nm)); // create the html link
			};
		};

		name=class.asString;
		//name[0]=name[0].toUpper; // The class name from the key

		string=template.replace("%%",links); //Insert the list of  links into the template

		string=string.replace("%",name); // Insert the class name into the template

		file.write(string);
		file.close;

	}


}


}
