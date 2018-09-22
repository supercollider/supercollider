TestPlotter : UnitTest {

	test_set_plotColors_and_superpose {
		var data, cols, plot;

		data = 3.collect{15.collect{rrand(0.0, 3.0)}};
		cols = 3.collect{Color.rand};

		// set each plot to unique color, then superpose
		plot = data.plot.plotColors_(cols);
		plot.superpose_(true);

		this.assertEquals(
			plot.plots[0].plotColor,
			cols,
			format(
				"The color of the superposed plot:\n\t[%]\n"
				"\tshould match Plotter's plotColors (in order) when superpose = true.:\n\t[%]\n",
				plot.plots[0].plotColor,
				cols,
			), report: false
		);

		// plot individually
		plot.superpose_(false);

		this.assertEquals(
			plot.plots.collect({|plt, i| plt.plotColor[0]}),
			cols,
			format(
				"The first color of each individual plot:\n\t[%]\n"
				"\tshould match Plotter's plotColors (in order) when superpose = false.:\n\t[%]\n",
				plot.plots.collect({|plt, i| plt.plotColor[0]}),
				cols,
			), report: false
		);

		plot.parent.close;
	}

}