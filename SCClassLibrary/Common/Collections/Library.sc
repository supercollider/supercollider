MultiLevelIdentityDictionary : Collection {

	var <>dictionary;

	*new {
		^super.new.init
	}
	init {
		dictionary = this.newInternalNode
	}

	newInternalNode { ^this.nodeType.new }

	nodeType {
		^IdentityDictionary
	}

	at { |... path|
		^this.atPath(path)
	}
	atPathFail { |path, function|
		var item;
		item = dictionary;
		path.do { |name|
			item = item.at(name);
			if(item.isNil) { ^function.value }
		};
		^item
	}
	atPath { |path|
		^this.atPathFail(path)
	}

	put { |... path|
		var item;
		item = path.pop;
		^this.putAtPath(path, item)
	}
	putAtPath { |path, val|
		var item, lastName;
		path = path.copy;
		lastName = path.pop;
		item = dictionary;
		path.do { |name|
			item = item.atFail(name, {
				var newitem;
				newitem = this.newInternalNode;
				item.put(name, newitem);
				newitem
			})
		};
		item.put(lastName, val)
	}

	create { |... args|
		var item;
		item = dictionary;
		args.do { |name|
			item = item.atFail(name, {
				var newitem;
				newitem = this.newInternalNode;
				item.put(name, newitem);
				newitem
			})
		}
	}

	choose { |... start|
		var item;
		if(start.isEmpty) {
			item = dictionary
		} {
			item = this.at(*start);
			if(item.isNil) {
				Error("Library-choose start address not found: " ++ start).throw
			}
		};
		^this.prChooseFrom(item)
	}
	putTree { |... items|
		this.prPutTree([], items)
	}
	postTree { |obj, tabs = 0|
		if(obj.isNil) { obj = dictionary };
		if(obj.isKindOf(this.nodeType)) {
			"".postln;
			obj.keysValuesDo({ |k, v|
				tabs.do { Char.tab.post };
				k.post;
				": ".post;
				this.postTree(v, tabs + 1)
			})
		} {
			Char.tab.post;
			obj.asString.postln
		}
	}
	do { |function|
		dictionary.do(function)
	}
	// remove only the leaf node indicated by path
	// parent nodes remain in the MLID even if they are empty
	removeAt { |... path|
		^this.removeAtPath(path)
	}
	removeAtPath { |path|
		var item, lastName;
		path = path.copy;
		lastName = path.pop;
		item = dictionary;
		path.do { |name|
			item = item.at(name);
			if(item.isNil) { ^nil }
		};
		^item.removeAt(lastName)
	}
	// remove the leaf node
	// as well as parent nodes that become empty after removing the child
	// slower but leaves less cruft in the tree
	prRemoveAtPathRecursive { |path, i = 0, item|
		var name = path[i], result;
		if(item[name].isNil) { ^nil };
		if(i < (path.size - 1)) {
			result = this.prRemoveAtPathRecursive(path, i + 1, item[name]);
			if(item[name].isEmpty) { item.removeAt(name) };
			^result
		} {
			^item.removeAt(name)
		}
	}
	removeEmptyAtPath { |path|
		^this.prRemoveAtPathRecursive(path, 0, dictionary)
	}
	removeEmptyAt { |...path|
		^this.prRemoveAtPathRecursive(path, 0, dictionary)
	}

	//private
	add { |assn|
		this.put(assn.key, assn.value)
	}
	remove { ^this.shouldNotImplement(thisMethod) }
	removeFail { ^this.shouldNotImplement(thisMethod) }

	prChooseFrom { |dict|
		var item;
		item = dict.choose;
		if(item.isKindOf(this.nodeType)) {
			^this.prChooseFrom(item)
		} {
			^item
		}
	}
	prPutTree { |keys, items|
		forBy(0, items.size - 1, 2) { |i|
			var key, item;
			key = items.at(i);
			item = items.at(i + 1);
			if(item.isArray.not) {
				this.put(* keys ++ [key, item])
			} {
				//array
				this.prPutTree(keys ++ [key], item)
			}
		}
	}
	leaves { |startAt|
		if(startAt.isNil) {
			startAt = dictionary
		} {
			startAt = this.at(*startAt)
		};
		^this.prNestedValuesFromDict(startAt)
	}
	prNestedValuesFromDict { |dict|
		^dict.values.collect { |thing|
			if(thing.isKindOf(this.nodeType)) {
				this.prNestedValuesFromDict(thing)
			} {
				thing
			}
		}
	}

	// Tree-like do methods
	leafDo { |func|

		this.doLeafDo([], this.dictionary, func)
	}
	leafDoFrom { |folderpath, func|
		var folder;

		folderpath = folderpath.asArray;

		folder = this.atPath(folderpath);
		if(folder.notNil && folder.isKindOf(this.nodeType)) {
			this.doLeafDo(folderpath, folder, func)
		}
	}

	doLeafDo { |path, object, func|

		if(object.isKindOf(this.nodeType)) {
			object.keysValuesDo({ |name, subobject|
				this.doLeafDo(path ++ [name], subobject, func)
			})
		} {
			func.value(path, object)
		}
	}

	treeDo { |branchFunc, leafFunc, argument0, postBranchFunc|
		var result;

		result = this.doTreeDo([], this.dictionary, branchFunc, leafFunc, argument0, postBranchFunc);
		^result
	}
	treeDoFrom { |folderpath, branchFunc, leafFunc, argument0, postBranchFunc|
		var folder, result;

		folderpath = folderpath.asArray;

		folder = this.atPath(folderpath);
		if(folder.isKindOf(this.nodeType)) {
			result = this.doTreeDo(folderpath, folder, branchFunc, leafFunc, argument0, postBranchFunc)
		} {
			result = nil
		};

		^result

	}
	doTreeDo { |path, object, branchFunc, leafFunc, argument, postBranchFunc|
		var result;

		if(object.isKindOf(this.nodeType)) {
			if(branchFunc.notNil) {
				result = branchFunc.value(path, object, argument)
			} {
				result = argument
			};
			object.keysValuesDo({ |name, subobject|
				this.doTreeDo(path ++ [name], subobject, branchFunc, leafFunc, result, postBranchFunc)
			});
			if(postBranchFunc.notNil) {
				result = postBranchFunc.value(path, object, result)
			};
			^result
		} {
			leafFunc.value(path, object, argument)
		}
	}

	treeCollect { |branchFunc, leafFunc, postBranchFunc|
		var result;
		result = this.doTreeCollect([], this.dictionary, branchFunc, leafFunc, postBranchFunc);
		^result
	}
	doTreeCollect { |path, object, branchFunc, leafFunc, postBranchFunc|
		var confirm, collection, result;

		if(object.isKindOf(this.nodeType)) {
			if(branchFunc.notNil) {
				#confirm, result = branchFunc.value(path, object)
			} {
				#confirm, result = [true, nil]
			};
			if(confirm) {
				collection = [];
				object.keysValuesDo({ |name, subobject|
					collection = collection.add(this.doTreeCollect(path ++ [name], subobject,
						branchFunc, leafFunc, postBranchFunc))
				});
				collection.removeAllSuchThat({ |item| item.isNil });
				if(postBranchFunc.notNil) {
					result = postBranchFunc.value(path, object, collection)
				} {
					result = nil
				};
				^result
			} {
				^nil
			}
		} {
			^leafFunc.value(path, object)
		}
	}

	sortedTreeDo { |branchFunc, leafFunc, argument0, postBranchFunc, sortFunc|
		var result;

		result = this.doSortedTreeDo([], this.dictionary, branchFunc, leafFunc, argument0, postBranchFunc, sortFunc);
		^result
	}
	doSortedTreeDo { |path, object, branchFunc, leafFunc, argument, postBranchFunc, sortFunc|
		var result;

		sortFunc = sortFunc ? { |a, b| a < b };

		if(object.isKindOf(this.nodeType)) {
			if(branchFunc.notNil) {
				result = branchFunc.value(path, object, argument)
			} {
				result = argument
			};
			object.sortedKeysValuesDo({ |name, subobject|
				this.doSortedTreeDo(path ++ [name], subobject, branchFunc, leafFunc, result, postBranchFunc, sortFunc)
			});
			if(postBranchFunc.notNil) {
				postBranchFunc.value(path, object, result)
			};
			^result
		} {
			leafFunc.value(path, object, argument)
		}
	}

	leafDoInBranch { |folderpath, function|
		var path, folder;

		folderpath = folderpath.asArray;

		folder = this.atPath(folderpath);
		if(folder.notNil && folder.isKindOf(this.nodeType)) {
			folder.keysValuesDo({ |name, object|
				if(object.isKindOf(this.nodeType).not) {
					function.value(folderpath ++ [name], object)
				}
			})
		}
	}

	storeOn { |stream|
		stream << this.class.name << "[" <<<* dictionary << "]"
	}

	printOn { |stream|
		stream << this.class.name << "[" <<* dictionary << "]"
	}
}



LibraryBase : MultiLevelIdentityDictionary {
	*global {
		^this.subclassResponsibility(thisMethod)
	}
	*global_ { |obj|
		^this.subclassResponsibility(thisMethod)
	}


	*clear {
		this.global = this.new
	}
	*at { |... args|
		^this.global.at(*args)
	}

	*atList { |args|
		^this.global.at(*args)
	}
	*putList { |args|
		^this.global.put(*args)
	}

	*put { |... args|
		^this.global.put(*args)
	}
	*create { |... args|
		^this.global.create(*args)
	}

	*postTree {
		this.global.postTree
	}

}

Library : LibraryBase {
	classvar global;

	*global { ^global }
	*global_ { |obj| global = obj }

	*initClass {
		global = this.new
	}
}

Archive : LibraryBase {
	classvar global;
	classvar <>archiveDir;

	*global { ^global }
	*global_ { |obj| global = obj }

	*initClass {
		global = this.new;
		archiveDir = Platform.userAppSupportDir
	}

	*read { |filename|
		var expandedFileName;
		expandedFileName = filename ?? (archiveDir ++ "/archive.sctxar");
		if(File.exists(expandedFileName)) {
			if(expandedFileName.endsWith(".scar")) {
				global = this.readBinaryArchive(expandedFileName)
			} {
				global = this.readArchive(expandedFileName)
			};
			if(global.isNil) {
				global = this.new
			}
		}
	}
	*write { |filename|
		var expandedFileName;
		expandedFileName = filename ?? (archiveDir ++ "/archive.sctxar");
		global.writeArchive(expandedFileName)
	}
}
