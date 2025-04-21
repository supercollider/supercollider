PathName {

	var <fullPath;

	// later: move classvars to String or Platform and only alias here
	classvar <>scroot;
	classvar <>tmp;

	// returns a standardized string directly
	*new { | path = "" |
		// ^super.newCopyArgs(path.standardizePath);
		^path.standardizePath
	}

	*initClass {
		scroot = File.getcwd;
		tmp = Platform.defaultTempDir;
		if(tmp.isNil, {
			"No valid temp directory found. Please set it manually using PathName.tmp_".warn
		})
	}

	/****** redirections commented out to test direct string return approach *****/

	// asPath { ^fullPath }
	//
	// // old PathName methods, colon was Mac OS9 path separator...
	// colonIndices { ^fullPath.separatorIndices }
	// lastColonIndex { ^fullPath.lastSeparatorIndex }
	// // new better names
	// separatorIndices { ^fullPath.separatorIndices }
	// lastSeparatorIndex { ^fullPath.lastSeparatorIndex }
	//
	// fileName { ^fullPath.fileName }
	// fileNameWithoutExtension { ^fullPath.fileNameWithoutExtension }
	// fileNameWithoutDoubleExtension { ^fullPath.fileNameWithoutDoubleExtension }
	// extension { ^fullPath.extension }
	//
	// dirname { ^fullPath.dirname }
	// pathOnly { ^fullPath.pathOnly }
	//
	// diskName { ^fullPath.diskName }
	//
	// isRelativePath { ^fullPath.isRelativePath }
	// isAbsolutePath { ^fullPath.isAbsolutePath }
	// absolutePath { ^fullPath.absolutePath }
	//
	// asRelativePath { |relativeTo| ^fullPath.asRelativePath(relativeTo) }
	// asAbsolutePath { ^fullPath.asAbsolutePath }
	//
	// allFolders { ^fullPath.allFolders }
	//
	// folderName { ^fullPath.folderName }
	//
	// nextName { ^fullPath.nextName }
	// noEndNumbers { ^fullPath.noEndNumbers }
	// endNumber { ^fullPath.endNumber }
	// endNumberIndex { ^fullPath.endNumberIndex }
	//
	// == { |other| { ^fullPath.absolutePath }
	// 	^other.respondsTo(\fullPath) and: { other.fullPath == this.fullPath }
	// }
	//
	// /* concatenation */
	// +/+ { | path |
	// 	var otherFullPath = path.respondsTo(\fullPath).if({ path.fullPath }, { path.asString });
	// 	^this.class.new(fullPath +/+ otherFullPath)
	// }
	//
	// pathMatch { ^fullPath.pathMatch }
	// entries { ^fullPath.entries }
	// isFolder { ^fullPath.isFolder }
	// isFile { ^fullPath.isFile }
	//
	// files { ^fullPath.files }
	// folders { ^fullPath.folders }
	// deepFiles { ^fullPath.deepFiles }
	//
	// parentPath { ^fullPath.parentPath }
	//
	// filesDo { |func| ^fullPath.filesDo(func) }
	//
	// streamTree { | str, tabs = 0 | fullPath.streamTree(str, tabs) }
	// dumpTree { fullPath.dumpTree }
	// printOn { | stream |
	// 	stream << "PathName(" << fullPath.cs << ")"
	// }
	// dumpToDoc { | title="Untitled" | fullPath.dumpToDoc(title) }
}
