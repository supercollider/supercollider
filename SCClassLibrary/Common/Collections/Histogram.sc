Histogram  {
    var <data, <n_bins, <min, <max, <x_warp; //these have dedicated setters further down
    var <freqsArray;
    var <domain;
    var <domainSpec;
    var <yspec;
    var <outliers;
    var <plotter;

    *new { | data, n_bins, min, max, x_warp(\lin) |
		if(data.isEmpty) { MethodError("Histogram.new: data is empty").throw; };
        ^super.newCopyArgs(data, n_bins, min, max, x_warp)
            .prUpdateHistogram;
	}

    prUpdateHistogram {
        domainSpec = this.prPrepareDomainSpec;
        if(data.maxDepth == 1) {
            #freqsArray, outliers = this.prMakeFreqsArray(data);
        } {
            #freqsArray, outliers = data.collect (this.prMakeFreqsArray(_)).flop;
        };
        domain = domainSpec.map(Array.series(n_bins, 0, 1/(n_bins)));
        // the values in domain give the lower bounds of each bins.
        // this means that domain.last will be < max.
        yspec = [0, freqsArray.flat.maxItem * 1.05, \lin].asSpec;
        plotter !? { this.plot };
        ^this
    }

    prPrepareDomainSpec {
        // flat copy: mem abuse?
        var range, binWidth, new_n_bins, new_max, dataflat = data.asArray.flat;
        n_bins = n_bins ?? { data.shape.last.sqrt.ceil };
        min = min ?? { dataflat.minItem };
		max = max ?? { dataflat.maxItem };
        range = max - min;
        if(x_warp === \lin or: { x_warp === \linear } and: { dataflat.every(_.isInteger) } ) {
            range = range + 1; // this +1 is to include the max (e.g.: 4 - 2 = 2, but interval [2, 4] includes 3 integers)
            if (n_bins >= range) {
                n_bins = range.asInteger;
            } { // binWidth should be integer.
                // if not, there may be artifacts where certain bins contain more integers than others
                // e.g.: the intervals (bins) [1.9, 3.1) and  [2.1, 3.3), are of the same size,
                // but the former contains 2 integer values (2 and 3); contains only 1 (3);
                // first, establish bin width closest to specified extremes and n_bins:
                if (not(range.isPrime)) {
                    // test if other possible values for n_bins result in Integer binWidth
                    binWidth = range/(n_bins + ([0] ++ (1..n_bins.div(4)) *.x [1, -1]));
                    binWidth = binWidth.detect { |x| x.floor == x }
                };
                binWidth = binWidth ?? { (range/n_bins).round.asInteger };
                // we may in turn have to round n_bins as well:
                new_n_bins = (range/binWidth).round.asInteger;
                // update max accordingly
                new_max = new_n_bins * binWidth + min - 1;
                if ( new_max != max or: { new_n_bins != n_bins } ) {
                    n_bins = new_n_bins;
                    max = new_max;
                    postln("Histogram: adjustments for integer domain: \n"
                        "adjusted max and/or n_bins to permit integer bin width and avoid artifacts.\n"
                        "bin width = %; width of rightmost bin may still be larger by 1. \n"
                        "using n_bins = %, max = %".format(binWidth, n_bins, max))
                }
            }
        };
        ^[min, max, x_warp].asSpec;
    }

    prMakeFreqsArray { | data1d |
		var freqs, freqIndex, outliers1d;
		freqs = 0.dup(n_bins);
        outliers1d = 0;

        data1d.do { | x |
            freqIndex = (domainSpec.unmap(x) * n_bins).asInteger;
            // Note that (only) when x >= max, this will output n_bins,
            // even though the last index of freqs is n_bins - 1.
            // this is intentional; the last bin interval is closed on both sides,
            // i.e., it should include the max, and this slight asymmetry requires special treatment.
            // special cases:
            // 1. freqIndex == n_bins; this happens when x == max and with outliers (i.e., x > max)
            // 2. freqIndex == 0; this happens when x is in the first bin (not just when x == min !), and with outliers (x < min).
            case
                {freqIndex.exclusivelyBetween(0, n_bins)} { freqs[freqIndex] = freqs[freqIndex] + 1 }
                { x == max } { freqs[n_bins-1] = freqs[n_bins-1] + 1 }
                { x > max } { outliers1d = outliers1d + 1 }
                { x >= min } { freqs[0] = freqs[0] + 1 }
                { outliers1d = outliers1d + 1 }
        };

        if (outliers1d > 0) {
                postf("Histogram : % of % values in the collection are out of the histogram range [%, %].\n",
                outliers1d, data1d.size, min, max)
        };
		^[freqs, outliers1d];
    }


    data_ { | new_data |
        data = Histogram.prCheckdata(new_data);
        this.prUpdateHistogram
    }

    n_bins_ { | new_n_bins |
        n_bins = new_n_bins;
        this.prUpdateHistogram
    }

    min_ { | new_min |
        min = new_min;
        this.prUpdateHistogram
    }

    max_ {| new_max |
        max = new_max;
        this.prUpdateHistogram
    }

    x_warp_ {| new_warp |
        x_warp = new_warp;
        this.prUpdateHistogram
    }

    plot { | name, bounds, parent |
        // if a plotter already exists, update its values rather than making a new one.
        plotter = plotter !? { plotter.value = freqsArray } ?? { freqsArray.plot(name, bounds, parent: parent) };
        plotter
            .name_(name ?? { plotter.name })
            .bounds_(bounds ?? { plotter.bounds })
            .domainSpecs_([domainSpec])
            .domain_(domain)
            .specs_([yspec])
            .plotMode_(\bars)
            .axisLabelY_("Occurrences")
            .axisLabelX_("Bins")
            .refresh;
        ^plotter;
    }

}

+ Collection {
    // from Collection
    histo { |n_bins, min, max, x_warp|
        ^Histogram.new(this, n_bins, min, max, x_warp)
    }
    // from PlotView.sc
    plotHisto { |n_bins, min, max, x_warp|
        ^Histogram.new(this, n_bins, min, max, x_warp).plot;
    }
}
