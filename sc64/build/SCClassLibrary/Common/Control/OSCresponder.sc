OSCresponder {
	classvar <>all;
	var <>addr, <>cmdName, <>action;
	
	*new { arg addr, cmdName, action;
		^super.newCopyArgs(addr, cmdName.asSymbol, action);
	}
	
	*initClass {
		all = Set.new;
	} 
	
	*respond { arg time, addr, msg;
		var cmdName, responder, match;
		
		#cmdName = msg;
		responder = this.new(addr, cmdName);
		match = all.findMatch(responder);
		if (match.isNil, { 
			responder.addr = nil;
			match = all.findMatch(responder);
			if (match.isNil, { ^false });
		});
		match.value(time, msg, addr); 
		^true
	}
	*add { arg responder;
		var old;
		old = all.findMatch(responder);
		if(old.notNil,{ all.remove(old) });
		all.add(responder);
	}
	*remove { arg responder;
		all.remove(responder);
	}
	*removeAddr { arg addr;
		all = all.reject({ arg item;
			item.addr == addr
		});
	}
	
	value { arg time, msg, addr;
		action.value(time, this, msg, addr);
	}
	== { arg that;
		^that respondsTo: #[\cmdName, \addr]
			and: { cmdName == that.cmdName and: { addr == that.addr }}
	}
	hash {
		^addr.hash bitXor: cmdName.hash
	}
	add { this.class.add(this); }
	remove { all.remove(this) }
	removeWhenDone {
		var func;
		func = action;
		action = { arg time, responder, msg, addr;
			func.value(time, responder, msg, addr);
			this.remove;
		}
	}
}


//used to manage OSCresponderNodes. do not use directly.

OSCMultiResponder : OSCresponder {
	var <>nodes;
	
	value { arg time, msg;
		var iterlist;
		iterlist = nodes.copy;
		iterlist.do({ arg node; node.action.value(time, node, msg) });
	}
	isEmpty { ^nodes.size == 0 }
	
}


OSCresponderNode {
	var <addr, <cmdName, <>action;
	*new { arg addr, cmdName, action;
		^super.newCopyArgs(addr, cmdName.asSymbol, action);
		
	}
	//i.zannos fix
	add {
		var made, found;
		made = OSCMultiResponder(addr, cmdName);
		found = OSCresponder.all.findMatch(made);
		if(found.isNil, { made.nodes = [this]; made.add; ^this });
		if (found.class === OSCresponder, {
			found.remove;
			made.nodes = [found, this];
			made.add;
		},{
			found.nodes = found.nodes.add(this)
		});
	}
	
	removeWhenDone {
		var func;
		func = action;
		action = { arg time, responder, msg, addr;
			func.value(time, responder, msg, addr);
			this.remove;
		}
	}
	
	remove { 
		var resp, alreadyThere;
		resp = OSCMultiResponder(addr, cmdName);
		alreadyThere = OSCresponder.all.findMatch(resp);
		if(alreadyThere.notNil) 
		{ 
			alreadyThere.nodes.remove(this);
			if(alreadyThere.isEmpty, { alreadyThere.remove });
		}; 
	}
	
	value { arg time, msg, addr;
		action.value(time, this, msg, addr);
	}

}

