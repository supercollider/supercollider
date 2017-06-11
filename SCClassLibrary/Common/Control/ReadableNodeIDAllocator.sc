/* make nodeIDs more humanly readable:
10 ** 8 ids for 1 user, 10 ** 7 for < 21, 10 ** 6 for < 210

for example, with 20 users, clientIDs would be
clientID  defaultGroup  tempIDs
 1         10000001      120001000, 10001001, etc
 2         10000001      120001000, 10001001, etc
12        120000001      120001000, 10001001, etc

In multi-client settings, this makes s.plotTree display
or s.queryAllNodes posts much easier to understand.
*/

ReadableNodeIDAllocator {

	var <userID, <lowestTempID, <numUsers;
	var <numIDs, <idOffset, <maxID, <numPerm, <maxPermID;
	var tempCount = -1, permCount = 1, permFreed;

	*new { arg userID = 0, lowestTempID = 1000, numUsers = 32;
		^super.newCopyArgs(userID, lowestTempID, numUsers).reset
	}

	reset {
		var maxNumPerUser = (2 ** 31 / numUsers).trunc.postln;
		// go to next lower power of 10:
		numIDs = (10 ** maxNumPerUser.log10.trunc).asInteger;

		idOffset = numIDs * userID;

		permFreed = IdentitySet.new;
		maxPermID = idOffset + lowestTempID - 1;
		tempCount = -1;
		permCount = 1;
	}

	alloc {
		tempCount = tempCount + 1;
		// wraps after 10**7 or so ids
		^(lowestTempID + tempCount).wrap(lowestTempID, numIDs) + idOffset;
	}

	isPerm { |num|
		// 0 and 1 are also permanent
		^num.inclusivelyBetween(idOffset, maxPermID);
	}

	allocPerm {
		var perm;
		if(permFreed.size > 0) {
			perm = permFreed.minItem;
			permFreed.remove(perm);
			^perm
		};

		permCount = (permCount + 1).min(lowestTempID);
		perm = permCount;
		if (perm >= lowestTempID) {
			warn("%: cannot create more than % permanent ids."
				"\nPlease free some permanent ids first,"
				"or set lowestTempID higher."
				.format(thisMethod, perm)
			);
			^nil
		};
		^perm + idOffset
	}

	freePerm { |id|
		if (id.isPerm) { permFreed.add(id) }
	}
}
