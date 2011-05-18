//Copyright 2007 Alex Norman
//with modifications 2008 Dan Stowell
//
//This file is part of SCVIM.
//
//SCVIM is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//SCVIM is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with SCVIM.  If not, see <http://www.gnu.org/licenses/>.

SCVim {

classvar <scvim_dir, <scvim_cache_dir,
	// autoFirstRun is whether we should attempt to create the caches if they're not found
	<>autoFirstRun=true;

*initClass {
	var scvim_cache_dir_env = getenv("SCVIM_CACHE_DIR");
	if(scvim_cache_dir_env.isNil){
		scvim_cache_dir = "~/.scvim".standardizePath;
		setenv("SCVIM_CACHE_DIR", scvim_cache_dir);
	}{
		scvim_cache_dir = scvim_cache_dir_env;
   };
   StartUp.add{
      if(Platform.ideName=="scvim"){ 
         if(autoFirstRun and:{ File.exists(scvim_cache_dir).not }){
            Task{
               "SCVim: generating help docs, it will take a few moments. (This only happens the first time you launch scvim. See SCVim help file for more info.)".inform;
               this.updateCaches;
               this.updateHelpCache;
               "SCVim: finished generating help docs".inform;
            }.play;
         };
      };
   };
}


*updateCaches {

	var object_list, object_dict;
	var ob_vim_file, ob_comp_file, tags_file;

	r{
		//if scvim_cache_dir doesn't exist, make it
		if(File.exists(scvim_cache_dir +/+ "doc").not){
			("mkdir -p" + scvim_cache_dir +/+ "doc").systemCmd;
		};

		//open the files
		//for supercollider_objects.vim
		ob_vim_file = File(scvim_cache_dir +/+ "supercollider_objects.vim","w");
		//for TAGS_SCDEF (object definitions)
		tags_file = File(scvim_cache_dir +/+ "TAGS_SCDEF","w");
		//for object completion && object lookup in the SChelp function
		ob_comp_file = File(scvim_cache_dir +/+ "sc_object_completion","w");

		ob_vim_file.write("syn keyword\tscObject\t");
		object_list = SortedList.new(0);
		object_dict = Dictionary.new;
		//add Object itself
		object_list = object_list.add(Object.asString);
		object_dict.add(Object.asString -> Object);
		//sort the Objects (add to a sorted list)
		Object.allSubclasses.do(
			{|i|
				object_list = object_list.add(i.asString);
				object_dict.add(i.asString -> i);
			});
		//go through the Objects in order and write what needs to be written to the files
		object_list.do {|ob_string|
				ob_vim_file.write(ob_string ++ " ");
				/* disregard any Meta_ Classes */
				if(ob_string.find("Meta_",false,0).isNil,
					{ob_comp_file.write(ob_string ++ "\n")});
				tags_file.write("SCdef:" ++
						ob_string ++
						"\t" ++ object_dict.at(ob_string).filenameSymbol ++
						"\tgo " ++
						(object_dict.at(ob_string).charPos + 1) ++ "\n");
			};
		//add some extra new lines
		ob_vim_file.write("\n\n");
		[ob_vim_file, tags_file, ob_comp_file].do(_.close);
		"SCVim files written".postln;
	}.play;
} // end *updateCaches

*updateHelpCache { | helpPaths |
    var getFiles, createHelp, objHelpPath, docDir, tagsDict, makeHelpFile, plain_text, new_path;

    r{
        //TODO currently ignoring helpPaths..
        //if(helpPaths.isNil){ helpPaths = [Platform.helpDir]};

        docDir = SCVim.scvim_cache_dir ++ if((SCVim.scvim_cache_dir.last == "/"), { "doc" }, { "/doc" });
        tagsDict = Dictionary.new;

        objHelpPath = { |obj|
            if((obj == Object), { "Object" }, { objHelpPath.value(obj.superclass) ++ "/" ++ obj.asString; });
        };

        makeHelpFile = { |dest_file, dest_path, source_file|
            if(File.exists(dest_path).not) {
                ("mkdir -p " ++ dest_path).systemCmd;
            };
            plain_text = File.use(source_file, "r") { |f| 
                switch(source_file.splitext[1],
                        "html", { f.readAllStringHTML },
                        "htm", { f.readAllStringHTML },
                        "rtf", { f.readAllStringRTF },
                        "scd", { f.readAllString },
                        { Error("unsupported file format " ++ source_file).throw; }
                      );
            }.replace("&lt;", "<").replace("&gt;", ">").replace("&amp;", "&").replace("&nbsp;", " ");
            dest_file = dest_path ++ "/" ++ dest_file;
            try { File.use(dest_file, "w") { |out| out << plain_text }; }
            { |error|
                ("could not write file " ++ dest_file).postln;
            };
            dest_file;
        };

        createHelp = { |subj, path|
            if(File.exists(path)) {
                if((subj.class.asString.contains("Meta_")), {
                        if((subj == Object), {new_path = docDir},
                            {new_path = docDir ++ "/" ++ objHelpPath.value(subj.superclass)});
                        tagsDict[subj.asString] = makeHelpFile.value(subj.asString ++ ".scd", new_path, path);
                    },
                    {
                        new_path = docDir ++ "/other";
                        subj = subj.asString.replace(" ", "_");
                        tagsDict[subj.asString] = makeHelpFile.value(subj ++ ".scd", new_path, path);
                    });
            };
        };

        getFiles = { |collection|
            collection.keysValuesDo { |key, value|
                case
                { value.class == String } { createHelp.value(key,value); }
                { value.class == Dictionary } { getFiles.value(value); }
                ;
            };
        };

        postln("SCVim: processing help docs, this takes a little while....");
        ("mkdir -p " ++ docDir).systemCmd;
        getFiles.value(Help.tree);

        //add the scvim doc if it doesn't already exist
        if((File.exists(docDir ++ "/" ++ "SCVim.scd") && tagsDict.keys.asArray.includesAny([SCVim, "SCVim"]).not),
                { tagsDict["SCVim"] = (docDir ++ "/" ++ "SCVim.scd") }
          );

        //create the help completion and tags file
        File.use(docDir ++ "/sc_help_completion", "w") { |completion_file|
            File.use(docDir ++ "/TAGS_HELP", "w") { |tags_file|
                tagsDict.keys.asArray.sort.do { |t|
                    tags_file << ("SC:" ++ t ++ "\t" ++ tagsDict[t] ++ "\t/^\n");
                    completion_file << (t ++ "\n");
                };
            };
        };
        postln("..done");
    }.play;
}

} // end class

