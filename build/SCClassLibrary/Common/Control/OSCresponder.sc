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
		match.value(time, msg); 
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
	
	value { arg time, msg;
		action.value(time, this, msg);
	}
	== { arg that;
		^(cmdName == that.cmdName) and: { addr == that.addr }
	}
	hash {
		^addr.hash bitXor: cmdName.hash
	}
	add { this.class.add(this); }
	remove { all.remove(this) }
	removeWhenDone {
		var func;
		func = action;
		action = { arg time, msg; 
			func.value(time, msg); 
			this.remove;
		}
	}
}


//used to manage OSCresponderNodes. do not use directly.

OSCMultiResponder : OSCresponder {
	var <>nodes;
	
	value { arg time, msg;
		nodes.do({ arg node; node.action.value(time, this, msg) });
	}
	
	
}


OSCresponderNode {
	var <addr, <cmdName, <>action;
	*new { arg addr, cmdName, action;
		^super.newCopyArgs(addr, cmdName.asSymbol, action);
		
	}
	
	add {
		var resp, alreadyThere;
		resp = OSCMultiResponder(addr, cmdName);
		alreadyThere = OSCresponder.all.findMatch(resp);
		if(alreadyThere.isNil, { alreadyThere = resp; alreadyThere.add });
		alreadyThere.nodes = alreadyThere.nodes.add(this);
	}
	
	removeWhenDone {
		var func;
		func = action;
		action = { arg time, msg; 
			func.value(time, msg); 
			this.remove;
		}
	}
	
	remove { 
		var resp, alreadyThere;
		resp = OSCMultiResponder(addr, cmdName);
		alreadyThere = OSCresponder.all.findMatch(resp);
		if(alreadyThere.notNil, { 
			if(alreadyThere.nodes.size == 1, { 
				alreadyThere.remove 
			},{
				alreadyThere.nodes.remove(this);
			});
		}); 
	}
	
	value { arg time, msg;
		action.value(time, msg)
	}

}

