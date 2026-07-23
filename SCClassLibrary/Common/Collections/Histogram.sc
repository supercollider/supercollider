Histogram  {
    var <data, <n_bins, <min, <max, <x_warp; //these have dedicated setters further down
    var <freqsArray;
    var <domain;
    var <domainSpec;
    var <yspec;
    var <outliers;
    var <plotter;

    *new { | data, n_bins, min, max, x_warp(\lin) |
		if(data.isEmpty) { MethodError("Histogram.new: data is empty"); ^nil };
        data = Histogram.prCheckData(data);
        ^super.newCopyArgs(data, n_bins, min, max, x_warp)
            .prUpdateHistogram;
	}

    *prCheckData { |data|
        if(not(data[0].isCollection)) {
            data = [data]; // prepare for loop in prMakeFreqsArray
        } {
            if(data.collect(_.size).differentiate[1..].sum > 0) {
                warn("Histogram.new: data arrays not of same sample size; \
                    comparison may not be sensible")
            };
        };
        ^data
    }

    prUpdateHistogram {
        domainSpec = this.prPrepareDomainSpec;
        freqsArray = this.prMakeFreqsArray;

        // the data points should align with the middle of each bin,
        // not at its left boundary.
        // so we offset the Plotter's domain by half a bin width:
        // UNLESS the plotmode does this already!? --> Verify
        domain = domainSpec.map(Array.series(n_bins, 1 / (2  * n_bins), 1/n_bins));
        yspec = [0, freqsArray.flat.maxItem * 1.05, \lin].asSpec;
        plotter !? { this.plot };
        ^this
    }

    prPrepareDomainSpec {
        var range, binWidth;
        n_bins = n_bins ?? { data[0].size.sqrt.ceil };
        min = min ?? { data[0].minItem };
		max = max ?? { data[0].maxItem };
        range = max - min;
        if(data.every(_.every(_.isInteger)) and: { x_warp === \lin }) {
            if (n_bins > range) {
                n_bins = range.asInteger
            } { // binWidth needs to be integer;
                // so we round up n_bins
                binWidth = (range/n_bins).ceil;
                n_bins = (range/binWidth).ceil.asInteger;
                // update max accordingly
                max = n_bins * binWidth + min;
            }
        }
        ^[min, max, x_warp].asSpec;
    }

    prMakeFreqsArray {
		var freqs, freqIndexArray, lastIndex;
		lastIndex = n_bins - 1;
		freqs = 0.dup([data.size, n_bins]);
        outliers = 0.dup(data.size);

        data.do { | dataset, datasetIndex |
            freqIndexArray = (domainSpec.unmap(dataset) * lastIndex).floor.asInteger ;
		    freqIndexArray.do { |freqIndex, i|
                // freqIndex is now constrained (by the unmapping) to [0, lastIndex];
                // so we need to distinguish clipped outliers from actual first/last bin values
                if (freqIndex != 0 and: { freqIndex != lastIndex } or: { dataset[i].inclusivelyBetween(min, max) } ) {
                    freqs[datasetIndex][freqIndex] = freqs[datasetIndex][freqIndex] + 1;
                } { // else it is an outlier.
                    outliers[datasetIndex] =  outliers[datasetIndex] + 1;
                };
            };
        };

        if (outliers.any(_ > 0)) {
                data.size.do { |i|
                    postf("Histogram : % of % values in the collection are out of the histogram range [%, %].\n",
                    outliers[i], data[i].size, min, max)
                }
        };

		^freqs;
	}

    data_ { | new_data |
        data = Histogram.prCheckdata(new_data);
        this.prUpdateHistogram
    }

    n_bins_ {| new_n_bins |
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

    plot {
        // if a plotter already exists, update its values rather than making a new one.
        plotter = plotter !? { plotter.value = freqsArray } ?? { freqsArray.plot };
        plotter.domainSpecs_([domainSpec])
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
