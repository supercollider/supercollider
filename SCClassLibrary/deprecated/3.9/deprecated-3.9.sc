SplayZ {
    *ar { arg numChans = 4, inArray, spread = 1, level = 1, width = 2, center = 0.0,
        orientation = 0.5, levelComp = true;

        var n = inArray.size.max(2);
        var n1 = n - 1;

        this.deprecated(thisMethod, SplayAz.class.findMethod(\ar));

        if (levelComp) { level = level * n.reciprocal.sqrt };

        ^Mix(PanAz.ar(
            numChans,
            inArray,
            ((0 .. n1) * (2 / n1) - 1) * spread + center,
            1,
            width,
            orientation
        )) * level;
    }

    *arFill { arg numChans = 4, n, function, spread = 1, level = 1, width = 2, center = 0.0,
        orientation = 0.5, levelComp = true;

        this.deprecated(thisMethod, SplayAz.class.findMethod(\arFill));

        ^this.ar(numChans, function ! n, spread, level, width, center,
            orientation, levelComp)
    }
}

SharedOut : AbstractOut {
    *kr { arg bus, channelsArray;
        this.deprecated(thisMethod, Bus.findMethod(\getSynchronous));
        this.multiNewList(['control', bus] ++ channelsArray.asArray)
        ^0.0        // Out has no output
    }
    *numFixedArgs { ^1 }
    writesToBus { ^false }
}

SharedIn : AbstractIn {
    *kr { arg bus = 0, numChannels = 1;
        this.deprecated(thisMethod, Bus.findMethod(\setSynchronous));
        ^this.multiNew('control', numChannels, bus)
    }
    init { arg numChannels ... argBus;
        inputs = argBus.asArray;
        ^this.initOutputs(numChannels, rate)
    }
}

+ PathName {
    *fromOS9 { | path | ^this.deprecated(thisMethod) }
    foldersWithoutCVS { | path | ^this.deprecated(thisMethod) }
    isCVS { ^this.deprecated(thisMethod) }
    foldersWithoutSVN { | path |^this.deprecated(thisMethod) }
    isSVN { ^this.deprecated(thisMethod) }
    filesDoNoCVS { | func | ^this.deprecated(thisMethod) }
    filesDoNoSVN { | func | ^this.deprecated(thisMethod) }
    streamTreeNoCVS { | str, tabs=0 | ^this.deprecated(thisMethod) }
    helpFilesDo { |func| ^this.deprecated(thisMethod) }
}

+ Server {
    recordNode {
        this.deprecated(thisMethod, Recorder.class.findMethod('recordNode'));
        ^recorder.recordNode
    }

    *set {
        this.deprecated(thisMethod, this.class.findMethod('all'));
        ^all
    }

    *set_ { |item|
        this.deprecated(thisMethod, this.class.findMethod('all_'));
        all = item
    }
}

+ Volume {
    numChans { ^this.deprecated(thisMethod, this.class.findMethod(\numChannels)) }
    numChans_ { ^this.deprecated(thisMethod, this.class.findMethod(\numChannels)) }
}

+ SimpleNumber {
	quantize { arg quantum = 1.0, tolerance = 0.05, strength = 1.0;
		var round = round(this, quantum);
		var diff = round - this;
		this.deprecated(thisMethod, SimpleNumber.findMethod('snap'));
		if(abs(diff) < tolerance) {
			^this + (strength * diff)
		} {
			^this
		}
	}
}

+ Server {
	userSpecifiedClientID {
		this.deprecated(thisMethod);
		^false
	}
}
