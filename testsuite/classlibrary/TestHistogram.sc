TestHistogram : UnitTest {
    test_data_recoverable {
        // not sure if this really needs to be tested for...
        // but maybe regression etc.
        var data = {2.0.rand}!25;
        this.assertEquals(data, data.histo.data,
            "Histogram should allow acces to the data used to generate it")
    }

    test_number_of_bins_smaller_than_number_of_datapoints {
        // a modest assumption. Won't hold if n = 1, sadly
        var n = 7;
        var data = {2.0.rand}!n;
        this.assert(data.histo.n_bins < n,
        "Histogram: when n_bins is automatically assigned (default), it should be smaller than number of datapoints")
    }


    test_auto_minmax_precludes_outliers {
        var data = {27.0.rand}.dup(100);

        this.assertEquals(data.histo.outliers, 0,
        "Histogram: when min and max are automatically assigned (default), no outliers should occur")
    }

    test_all_datapoints_accounted_for1 {
        var data = {7.0.rand}.dup(100);
        var histo = data.histo;

        this.assertEquals(data.size, histo.freqsArray.sum + histo.outliers,
        "Histogram: sum of frequencies plus outliers should equal number of data points")
    }

    test_all_datapoints_accounted_for2 {
        var data = {7.0.sum3rand}.dup(100);
        var histo = data.histo(25, 2, 5);

        this.assertEquals(data.size, histo.freqsArray.sum + histo.outliers,
        "Histogram: sum of frequencies plus outliers should equal number of data points")
    }

    test_avoid_integer_aliasing {
        var data = {rrand(10, 189)}.dup(170);
        var min = 10, max = 189; //
        var n_bins = 16; // this is close, but should get corrected to 15
        var histo = data.histo(n_bins, min, max);
        var binWidth = (histo.domain[1] - histo.domain[0]).round;
        this.assert( (histo.max - histo.min) == 179 and: { histo.n_bins == 15 } and: { binWidth == 12},
        "Histogram: when data is Integers, only allow integer bin sizes")
    }

    test_permit_change_of_histo_params_on_existing_data {
        var data = { rrand(0.001, 2000) }.dup(170);
        var histo = data.histo;
        var histoChanged = histo.copy().n_bins_(10).x_warp_(\exp).min_(20).max_(70);

        this.assertEquals(histo.data, histoChanged.data,
        "Histogram: changes in the histo parameters should not affect underlying data")
    }

    test_empty_data_error {
        var data = [];
        this.assertException({data.histo}, MethodError,
            "Histogram: empty data should throw error"
        )
    }

    test_domain_geometric {
        var data = Array.series(1000, 1, 29/999); //linearly uniform distro
        var n_bins = 10;
        var min = 1, max = 30;
        var x_warp = \exp; // but histo domain is exp
        var histo = data.histo(n_bins, min, max, x_warp);
        // expected result: higher bins get more hits than lower bins
        this.assert( histo.freqsArray.last > histo.freqsArray.first,
        "Histogram: when domain is geometrically scaled (x_warp=\exp), bins should also be allotted geometrically"
        )

    }
}
