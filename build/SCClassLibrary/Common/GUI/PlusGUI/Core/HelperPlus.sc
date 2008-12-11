+ Helper{

*updateRedirectStubs {
	var template, tfile, classesArray, path;
	path= path ? "Help/GUI/stubs/";
	
	tfile=File(path++"StubTemplate.html","r");  // read the html tmaplat file
	template=tfile.readAllString;
	tfile.close;
	
	classesArray = classesArray ?  ViewRedirecter.allSubclasses; 
	
	classesArray.do {arg class;
		var file, name, string, links="";
		
		(path++class.name.asString++".html").postln;
		
		file=File(path++class.name.asString++".html", "w"); //create the stub file
		
		GUI.schemes.do{arg scheme;  // for each registerd schme
			var nm, helpFilePath="";
			try{  // if this fails, then there is no gui class in the kit for this redirect.
			
				nm=scheme.perform(class.key).name.asString;  //Get the kit-specific class name
			
				{ helpFilePath=nm.findHelpFile
					.asRelativePath(Platform.helpDir++"/GUI/stubs")}.try({helpFilePath=nm++".html"});
					//if you can't find the helpfile path, then just create a file name
			
				links=links++(("<p class=\"p1\"><b>%:<span class=\"Apple-tab-span\">     </span>"
					 ++"</b><a href=\"%\">%</a></p><p class=\"p1\"></p>")
					.format(scheme ,helpFilePath,nm)); // create the html link
			};
		};
		
		name=class.key.asString; 
		name[0]=name[0].toUpper; // The class name from the key
		
		string=template.replace("%%",links); //Insert the list of  links into the template
		
		string=string.replace("%",name); // Insert the class name into the template

		file.write(string);
		file.close;
	
	}
		

}


}