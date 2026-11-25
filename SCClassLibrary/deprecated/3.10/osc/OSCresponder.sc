//////////////////////////////////////////////////////////////////////////////
//  _____________________________________
// / HEY YOU! When you're removing this  \
// | class, also make sure to remove the |
// | "OSCresponder.respond" line from    |
// \ Main:recvOSCmessage. Thanks!        /
//  -------------------------------------
// \                             .       .
//  \                           / `.   .' "
//   \                  .---.  <    > <    >  .---.
//    \                 |    \  \ - ~ ~ - /  /    |
//          _____          ..-~             ~-..-~
//         |     |   \~~~\.'                    `./~~~/
//        ---------   \__/                        \__/
//       .'  O    \     /               /       \  "
//      (_____,    `._.'               |         }  \/~~~/
//       `----.          /       }     |        /    \__/
//             `-.      |       /      |       /      `. ,~~|
//                 ~-.__|      /_ - ~ ^|      /- _      `..-'
//                      |     /        |     /     ~-.     `-. _  _  _
//                      |_____|        |_____|         ~ - . _ _ _ _ _>
//
//////////////////////////////////////////////////////////////////////////////

OSCresponder {
	classvar <>all;
	var <>addr, <cmdName, <>action;
	var <cmdNameWithoutSlash;

	*new { |addr, cmdName, action|
		this.deprecated(thisMethod, OSCFunc.class.findMethod(\new));
		^super.new.init(addr, cmdName, action)
	}

	*initClass {
		all = Set.new
	}

	init { |argAddr, argCmdName, argAction|
		addr = argAddr;
		action = argAction;
		this.cmdName = argCmdName
	}

	cmdName_ { |string|
		string = string.asString;
		if(string[0] == $/) { string = string.drop(1) };
		cmdNameWithoutSlash = string.asSymbol;
		cmdName = ("/" ++ string).asSymbol
	}


	*respond { |time, addr, msg|
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
				hit = true
			}
		};
		^hit
	}

	*add { |responder|
		var old;
		old = all.findMatch(responder);
		if(old.notNil) { all.remove(old) };
		all.add(responder)
	}
	*remove { |responder|
		all.remove(responder)
	}
	*removeAddr { |addr|
		all = all.reject({ |item|
			item.addr == addr
		})
	}

	value { |time, msg, addr|
		action.value(time, this, msg, addr)
	}
	== { |that|
		^that respondsTo: #[\cmdName, \addr]
			and: { cmdName == that.cmdName and: { addr == that.addr } }
	}
	hash {
		^addr.hash bitXor: cmdName.hash
	}
	add { this.class.add(this) }
	remove { all.remove(this) }
	removeWhenDone {
		var func;
		func = action;
		action = { |time, responder, msg, addr|
			func.value(time, responder, msg, addr);
			this.remove
		}
	}
}


//used to manage OSCresponderNodes. do not use directly.

OSCMultiResponder : OSCresponder {
	var <>nodes;

	value { |time, msg, addr|
		var iterlist;
		iterlist = nodes.copy;
		iterlist.do { |node| node.value(time, msg, addr) }
	}
	isEmpty { ^nodes.size == 0 }

}


OSCresponderNode : OSCresponder {

	var <added = false;

	add {
		var made, found;
		if(added.not) {
			made = OSCMultiResponder(addr, cmdName);
			found = all.findMatch(made);
			if(found.isNil) { made.nodes = [this]; made.add; added = true; ^this };
			if(found.class === OSCresponder) {
				found.remove;
				made.nodes = [found, this];
				made.add
			} {
				found.nodes = found.nodes.add(this)
			};
			added = true
		}
	}

	removeWhenDone {
		var func;
		func = action;
		action = { |time, responder, msg, addr|
			func.value(time, responder, msg, addr);
			this.remove
		}
	}

	remove {
		var resp, alreadyThere;
		resp = OSCMultiResponder(addr, cmdName);
		alreadyThere = all.findMatch(resp);
		if(alreadyThere.notNil) {
			alreadyThere.nodes.remove(this);
			if(alreadyThere.isEmpty) { alreadyThere.remove };
			added = false
		}
	}

	value { |time, msg, addr|
		action.value(time, this, msg, addr)
	}

}
