+ Helper{

*updateRedirectStubs {
	var template, tfile, classesArray, path, examplePath, extraString;
	path= path ? "Help/GUI/Main-GUI/";
	
	tfile=File(path++"StubTemplate.html","r");  // read the html template file
	template=tfile.readAllString;
	tfile.close;
	
	classesArray = classesArray ?  ViewRedirect.allSubclasses; 
	
	classesArray.do {arg class;
		var file, name, string, links="", stubPath, oldString;
		
		(path++class.name.asString++".html");
		stubPath = path++class.name.asString++".html";
		

		GUI.schemes.do{arg scheme;  // for each registered scheme
			var nm, helpFilePath="";
			try{  // if this fails, then there is no gui class in the kit for this redirect.
			
				nm=scheme.perform(class.key).name.asString;  //Get the kit-specific class name
			
				{ helpFilePath=nm.findHelpFile
					.asRelativePath(Platform.helpDir++"/GUI/Main-GUI")}.try({helpFilePath=nm++".html"});
					//if you can't find the helpfile path, then just create a file name
			
				links=links++(("<p class=\"p1\"><b>%:<span class=\"Apple-tab-span\">     </span>"
					 ++"</b> <a href=\"%\">%</a></p>\n<br>\n")
					.format(scheme ,helpFilePath,nm)); // create the html link
			};
		};
		
		name=class.asString; 
		//name[0]=name[0].toUpper; // The class name from the key
		
		
		extraString="<p class=\"p10\"><iframe height=800 frameborder=0 src=\"ExampleFiles/%-examples.html\"><a href=\"ExampleFiles/%-examples.html\">Examples and Usage</a></iframe></p>";

		string=template.replace("%%%",extraString); //Insert the list of  links into the template
		
		string=string.replace("%%",links); //Insert the list of  links into the template
		
		string=string.replace("%",name); // Insert the class name into the template
		
		string=string.replace("<iframe","<iframe width=100% "); // Insert the class name into the template
		
		examplePath="%/ExampleFiles/%-examples.html".format(stubPath.dirname, name).postln;
		
		if (File.exists(examplePath).not){
		
			file=File(examplePath, "w"); 
			file.write(CocoaGUI.perform(class.key).name.asString);
			file.close;
		};
		
		file=File(stubPath, "w"); //create the stub file
		file.write(string++"\n");
		file.close;
	
	}
		

}


}