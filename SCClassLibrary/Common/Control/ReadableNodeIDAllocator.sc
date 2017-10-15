/* make nodeIDs more humanly readable:
10 ** 8 ids for 1 user, 10 ** 7 for < 21, 10 ** 6 for < 210

for example, with 20 Clients, clientIDs would be
clientID  defaultGroup  tempIDs
1         10000001      120001000, 10001001, etc
2         10000001      120001000, 10001001, etc
12        120000001      120001000, 10001001, etc

In multi-client settings, this makes s.plotTree display
or s.queryAllNodes posts much easier to understand.
*/

ReadableNodeIDAllocator {

	var <clientID, <lowestTempID, <numClients;
	var <numIDs, <idOffset, <maxPermID;
	var tempCount = -1, permCount = 1, permFreed;

	*new { |clientID = 0, lowestTempID = 1000, numClients = 32|
		^super.newCopyArgs(clientID, lowestTempID, numClients).reset
	}

	reset {
		var maxNumPerUser = (2 ** 31 / numClients).trunc;
		// go to next lower power of 10:
		numIDs = (10 ** maxNumPerUser.log10.trunc).asInteger;

		idOffset = numIDs * clientID;

		permFreed = IdentitySet.new;
		maxPermID = idOffset + lowestTempID - 1;
		tempCount = -1;
		permCount = 1;
	}

	alloc {
		tempCount = tempCount + 1;
		// wraps after handing out (numIDs - lowestTempID) ids
		^(lowestTempID + tempCount).wrap(lowestTempID, numIDs) + idOffset;
	}

	isPerm { |num|
		// test whether num is a valid permanent nodeID for this client/allocator's range,
		// which includes idOffset + 0 (for clientID 0, RootNode)
		// and idOffset + 1, the defaultGroup for this allocator/client
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
		var topFreed;
		if (this.isPerm(id).not) { ^this };

		permFreed.add(id);
		while {
			permFreed.notEmpty and: {
				topFreed = permFreed.maxItem;
				topFreed == permCount;
			}
		} {
			permFreed.remove(topFreed);
			permCount = permCount - 1;
		}
	}
}
