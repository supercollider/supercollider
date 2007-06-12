DefStream : Pstr {
	var <parent;
	
	*new { arg parent;
		^super.new(parent.pattern).makeDependant(parent)
	}
	
	makeDependant { arg argParent;
		parent = argParent;
		parent.children.add(this);
	}
	
	prNext { arg inval;
		var val;
		val = stream.next(inval);
		if(val.isNil , {
			parent.children.remove(this);
		});
		^val
	}


}