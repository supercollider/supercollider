OSCresponder {
	classvar <>all;
	var <>addr, <cmdName, <>action;
	var <cmdNameWithoutSlash;

	*new { arg addr, cmdName, action;
		^super.new.init(addr, cmdName, action);
	}

	*initClass {
		all = Set.new;
	}

	init { arg argAddr, argCmdName, argAction;
		addr = argAddr;
		action = argAction;
		this.cmdName = argCmdName;
	}

	cmdName_ { arg string;
		string = string.asString;
		if(string[0] == $/) { string = string.drop(1) };
		cmdNameWithoutSlash = string.asSymbol;
		cmdName = ("/" ++ string).asSymbol;
	}


	*respond { arg time, addr, msg;
		var cmdName, hit = false;
		#cmdName = msg;
		all.do { |resp|
			if(
				(resp.cmdName === cmdName)
				or:
				{ resp.cmdNameWithoutSlash === cmdName }
				and: { addr.matches(resp.addr) }
			) {
				resp.value(time, msg, addr);
				hit = true;
			};
		};
		^hit
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

	value { arg time, msg, addr;
		var iterlist;
		iterlist = nodes.copy;
		iterlist.do({ arg node; node.value(time, msg, addr) });
	}
	isEmpty { ^nodes.size == 0 }

}


OSCresponderNode : OSCresponder {

	var <added = false;

	add {
		var made, found;
		if(added.not, {
			made = OSCMultiResponder(addr, cmdName);
			found = all.findMatch(made);
			if(found.isNil, { made.nodes = [this]; made.add; added = true; ^this });
			if (found.class === OSCresponder, {
				found.remove;
				made.nodes = [found, this];
				made.add;
			},{
				found.nodes = found.nodes.add(this)
			});
			added = true;
		})
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
		alreadyThere = all.findMatch(resp);
		if(alreadyThere.notNil)
		{
			alreadyThere.nodes.remove(this);
			if(alreadyThere.isEmpty, { alreadyThere.remove });
			added = false;
		};
	}

	value { arg time, msg, addr;
		action.value(time, this, msg, addr);
	}

}
