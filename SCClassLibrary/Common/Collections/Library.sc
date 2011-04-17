MultiLevelIdentityDictionary : Collection
{

	var <>dictionary;

	*new {
		^super.new.init
	}
	init {
		dictionary = this.newInternalNode;
	}

	newInternalNode { ^this.nodeType.new }

	nodeType {
		^IdentityDictionary;
	}

	at { arg ... path;
		^this.atPath(path)
	}
	atPathFail { arg path, function;
		var item;
		item = dictionary;
		path.do({ arg name;
			item = item.at(name);
			if (item.isNil, { ^function.value });
		});
		^item
	}
	atPath { arg path;
		^this.atPathFail(path)
	}

	put { arg ... path;
		var item;
		item = path.pop;
		^this.putAtPath(path, item);
	}
	putAtPath { arg path, val;
		var item, lastName;
		path = path.copy;
		lastName = path.pop;
		item = dictionary;
		path.do({ arg name;
			item = item.atFail(name, {
				var newitem;
				newitem = this.newInternalNode;
				item.put(name, newitem);
				newitem
			});
		});
		item.put(lastName, val);
	}

	create { arg ... args;
		var item;
		item = dictionary;
		args.do({ arg name;
			item = item.atFail(name, {
				var newitem;
				newitem = this.newInternalNode;
				item.put(name, newitem);
				newitem
			});
		});
	}

	choose { arg ... start;
		var item;
		if(start.isEmpty,{
			item = dictionary;
		},{
			item = this.at(*start);
			if(item.isNil,{
				Error("Library-choose start address not found: " ++ start).throw;
			});
		});
		^this.prChooseFrom(item);
	}
	putTree { arg ... items;
		this.prPutTree([],items)
	}
	postTree { arg obj,tabs=0;
		if(obj.isNil,{ obj = dictionary });
		if(obj.isKindOf(this.nodeType),{
			"".postln;
			obj.keysValuesDo({ arg k,v;
				tabs.do({ Char.tab.post });
				k.post;
				": ".post;
				this.postTree(v,tabs + 1)
			});
		},{
			Char.tab.post;
			obj.asString.postln;
		})
	}
	do { arg function;
		dictionary.do(function);
	}
		// remove only the leaf node indicated by path
		// parent nodes remain in the MLID even if they are empty
	removeAt {
		arg ... path;
		^this.removeAtPath(path)
	}
	removeAtPath { arg path;
		var item, lastName;
		path = path.copy;
		lastName = path.pop;
		item = dictionary;
		path.do({ arg name;
			item = item.at(name);
			if (item.isNil, { ^nil });
		});
		^item.removeAt(lastName);
	}
		// remove the leaf node
		// as well as parent nodes that become empty after removing the child
		// slower but leaves less cruft in the tree
	prRemoveAtPathRecursive { |path, i = 0, item|
		var name = path[i], result;
		if(item[name].isNil) { ^nil };
		if(i < (path.size-1)) {
			result = this.prRemoveAtPathRecursive(path, i+1, item[name]);
			(item[name].isEmpty).if({ item.removeAt(name) });
			^result
		} {
			^item.removeAt(name)
		};
	}
	removeEmptyAtPath { arg path;
		^this.prRemoveAtPathRecursive(path, 0, dictionary)
	}
	removeEmptyAt { arg ...path;
		^this.prRemoveAtPathRecursive(path, 0, dictionary);
	}

	//private
	add { arg assn;
		this.put(assn.key, assn.value);
	}
	remove { ^this.shouldNotImplement(thisMethod) }
	removeFail { ^this.shouldNotImplement(thisMethod) }

	prChooseFrom { arg dict;
		var item;
		item = dict.choose;
		if(item.isKindOf(this.nodeType),{
			^this.prChooseFrom(item);
		},{
			^item
		})
	}
	prPutTree { arg keys,items;
		forBy(0,items.size - 1,2,{ arg i;
			var key,item;
			key = items.at(i);
			item = items.at(i + 1);
			if(item.isArray.not,{
				this.put(* keys ++ [key,item]);
			},{
				//array
				this.prPutTree(keys ++ [key],item);
			})
		});
	}
	leaves { arg startAt;
		if(startAt.isNil,{
			startAt = dictionary;
		},{
			startAt = this.at(*startAt);
		});
		^this.prNestedValuesFromDict(startAt);
	}
	prNestedValuesFromDict { arg dict;
		^dict.values.collect({ arg thing;
			if(thing.isKindOf(this.nodeType),{
				this.prNestedValuesFromDict(thing)
			},{
				thing
			})
		})
	}

	// Tree-like do methods
	leafDo {
		arg func;

		this.doLeafDo([], this.dictionary, func);
	}
	leafDoFrom {
		arg folderpath, func;
		var folder;

		folderpath = folderpath.asArray;

		folder = this.atPath(folderpath);
		if (folder.notNil && folder.isKindOf(this.nodeType), {
			this.doLeafDo(folderpath, folder, func);
		});
	}

	doLeafDo {
		arg path, object, func;

		if (object.isKindOf(this.nodeType), {
			object.keysValuesDo({
				arg name, subobject;
				this.doLeafDo(path ++ [name], subobject, func)
			});
		}, {
			func.value(path, object);
		})
	}

	treeDo {
		arg branchFunc, leafFunc, argument0, postBranchFunc;
		var result;

		result = this.doTreeDo([], this.dictionary, branchFunc, leafFunc, argument0, postBranchFunc);
		^result;
	}
	treeDoFrom {
		arg folderpath, branchFunc, leafFunc, argument0, postBranchFunc;
		var folder, result;

		folderpath = folderpath.asArray;

		folder = this.atPath(folderpath);
		if (folder.isKindOf(this.nodeType), {
			result = this.doTreeDo(folderpath, folder, branchFunc, leafFunc, argument0, postBranchFunc);
		}, {
			result = nil;
		});

		^result;

	}
	doTreeDo {
		arg path, object, branchFunc, leafFunc, argument, postBranchFunc;
		var result;

		if (object.isKindOf(this.nodeType), {
			if (branchFunc.notNil, {
				result = branchFunc.value(path, object, argument);
			}, {
				result = argument;
			});
			object.keysValuesDo({
				arg name, subobject;
				this.doTreeDo(path ++ [name], subobject, branchFunc, leafFunc, result, postBranchFunc)
			});
			if (postBranchFunc.notNil, {
				result = postBranchFunc.value(path, object, result);
			});
			^result
		}, {
			leafFunc.value(path, object, argument);
		})
	}

	treeCollect { arg branchFunc, leafFunc, postBranchFunc;
		var result;
		result = this.doTreeCollect([], this.dictionary, branchFunc, leafFunc, postBranchFunc);
		^result;
	}
	doTreeCollect { arg path, object, branchFunc, leafFunc, postBranchFunc;
		var confirm, collection, result;

		if (object.isKindOf(this.nodeType), {
			if (branchFunc.notNil, {
				#confirm, result = branchFunc.value(path, object);
			}, {
				#confirm, result = [true, nil]
			});
			if (confirm, {
				collection = [];
				object.keysValuesDo({
					arg name, subobject;
					collection = collection.add(this.doTreeCollect(path ++ [name], subobject,
						branchFunc, leafFunc, postBranchFunc));
				});
				collection.removeAllSuchThat({arg item; item.isNil});
				if (postBranchFunc.notNil, {
					result = postBranchFunc.value(path, object, collection);
				}, {
					result = nil;
				});
				^result
			}, {
				^nil
			});
		}, {
			^leafFunc.value(path, object)
		});
	}

	sortedTreeDo {
		arg branchFunc, leafFunc, argument0, postBranchFunc, sortFunc;
		var result;

		result = this.doSortedTreeDo([], this.dictionary, branchFunc, leafFunc, argument0, postBranchFunc, sortFunc);
		^result;
	}
	doSortedTreeDo {
		arg path, object, branchFunc, leafFunc, argument, postBranchFunc, sortFunc;
		var result;

		sortFunc = sortFunc ? {arg a, b; a < b};

		if (object.isKindOf(this.nodeType), {
			if (branchFunc.notNil, {
				result = branchFunc.value(path, object, argument);
			}, {
				result = argument;
			});
			object.sortedKeysValuesDo({
				arg name, subobject;
				this.doSortedTreeDo(path ++ [name], subobject, branchFunc, leafFunc, result, postBranchFunc, sortFunc)
			});
			if (postBranchFunc.notNil, {
				postBranchFunc.value(path, object, result);
			});
			^result
		}, {
			leafFunc.value(path, object, argument);
		})
	}

	leafDoInBranch {
		arg folderpath, function;
		var path, folder;

		folderpath = folderpath.asArray;

		folder = this.atPath(folderpath);
		if (folder.notNil && folder.isKindOf(this.nodeType), {
			folder.keysValuesDo({
				arg name, object;
				if (object.isKindOf(this.nodeType).not, {
					function.value(folderpath ++ [name], object);
				});
			});
		});
	}

	storeOn { arg stream;
		stream << this.class.name << "[" <<<* dictionary << "]"
	}

	printOn { arg stream;
		stream << this.class.name << "[" <<* dictionary << "]"
	}
}



LibraryBase : MultiLevelIdentityDictionary
{
	*global {
		^this.subclassResponsibility(thisMethod);
	}
	*global_ { arg obj;
		^this.subclassResponsibility(thisMethod);
	}


	*clear {
		this.global = this.new;
	}
	*at { arg ... args;
		^this.global.at(*args);
	}

	*atList { arg args;
		^this.global.at(*args)
	}
	*putList { arg args;
		^this.global.put(*args)
	}

	*put { arg ... args;
		^this.global.put(*args)
	}
	*create { arg ... args;
		^this.global.create(*args)
	}

	*postTree {
		this.global.postTree
	}

}

Library : LibraryBase
{
	classvar global;

	*global { ^global }
	*global_ { arg obj; global = obj; }

	*initClass {
		global = this.new;
	}
}

Archive : LibraryBase
{
	classvar global;
	classvar <>archiveDir;

	*global { ^global }
	*global_ { arg obj; global = obj; }

	*initClass {
		global = this.new;
		archiveDir = Platform.userAppSupportDir;
	}

	*read { arg filename;
		var expandedFileName;
		expandedFileName = filename ?? (archiveDir ++ "/archive.sctxar");
		if (File.exists(expandedFileName)) {
			if (expandedFileName.endsWith(".scar")) {
				global = this.readBinaryArchive(expandedFileName);
			}{
				global = this.readArchive(expandedFileName);
			};
			if (global.isNil) {
				global = this.new;
			};
		}
	}
	*write { arg filename;
		var expandedFileName;
		expandedFileName = filename ?? (archiveDir ++ "/archive.sctxar");
		global.writeArchive(expandedFileName);
	}
}
