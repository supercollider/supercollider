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

