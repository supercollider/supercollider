// by Lance Putnam
// lance@uwalumni.com
// allows now to insert breakpoint on double-click by jan trutzschler

SCEnvelopeEdit : SCEnvelopeView {
	var <env, <pointsPerSegment, viewPoints;
	var <minLevel, <maxLevel, <minTime, <maxTime, totalDurRec, absTimes, numPoints;

	*viewClass { ^SCEnvelopeView }

	*new { arg parent, bounds, env, pointsPerSegment=10;
		^super.new(parent, bounds).initSCEnvelopeEdit(env, pointsPerSegment)
	}

	initSCEnvelopeEdit { arg argEnv, argPPS, setMinMax=true;
		env = argEnv;
		pointsPerSegment = argPPS.asInteger;
		if(setMinMax){
			minLevel = argEnv.levels.minItem;
			maxLevel = argEnv.levels.maxItem;
			if(minLevel == maxLevel){ minLevel = 0.0 };
		};
		minTime = 0;
		maxTime = env.times.sum;
		totalDurRec = 1/(maxTime - minTime);

		absTimes = Array.newClear(env.times.size + 1);
		absTimes[0] = 0;
		for(1, env.times.size, { arg i;
			absTimes[i] = absTimes[i-1] + env.times[i-1];
		});

		numPoints = (pointsPerSegment * env.times.size) + 1;  // add 1 for the last point
		viewPoints = Array.with(Array.newClear(numPoints), Array.newClear(numPoints));

		this
			.selectionColor_(Color.clear)
			.drawLines_(true)	// resize broken when no lines drawn
			.drawRects_(true)
		;
		this.mouseDownAction_{|view, x, y, modifiers, buttonNumber, clickCount|
			this.defaultMauseDownAction(x, y, modifiers, buttonNumber, clickCount);
		};
		this.action = { arg view;
			var bp, bpm1, bpp1, bpLevel, timePos;

			// if it's a breakpoint
			if((view.index % pointsPerSegment) == 0, {
				bp = view.index.div(pointsPerSegment);
				bpm1 = bp - 1;
				bpp1 = bp + 1;

				bpLevel = view.currentvalue.linlin(0.0, 1.0, minLevel, maxLevel);
				env.levels[bp] = bpLevel;

				timePos = view.value[0][view.index].linlin(0.0, 1.0, minTime, maxTime);

				// first breakpoint
				if(bp == 0, {
					if( timePos <= absTimes[bpp1], {
						env.times[bp] = absTimes[bpp1] - timePos;
						absTimes[bp] = timePos;
					},{ // going past right break point
						env.times[bp] = 0;
						absTimes[bp] = absTimes[bpp1];
					});
					this.updateSegment(bp);
				// end breakpoint
				},{ if(bp == env.times.size, {
					if( timePos >= absTimes[bpm1], {
						env.times[bpm1] = timePos - absTimes[bpm1];
						absTimes[bp] = timePos;
					},{	// going past left break point
						env.times[bpm1] = 0;
						absTimes[bp] = absTimes[bpm1];
					});
					this.updateSegment(bpm1);
				// a middle break point
				},{
					if(timePos > absTimes[bpp1], {	// past right break point
						env.times[bpm1] = absTimes[bp] - absTimes[bpm1];
						env.times[bp] = 0;
						absTimes[bp] = absTimes[bpp1];
					},{ if(timePos < absTimes[bpm1], { // past left break point
						env.times[bpm1] = 0;
						env.times[bp] = absTimes[bpp1] - absTimes[bp];
						absTimes[bp] = absTimes[bpm1];
					},{
						// set left segment dur
						env.times[bpm1] = timePos - absTimes[bpm1];

						// set right segment dur
						env.times[bp] = absTimes[bpp1] - timePos;

						absTimes[bp] = timePos;
					}); });
					this.updateSegment(bpm1);
					this.updateSegment(bp);
					if((timePos <= absTimes[bpp1]) && (timePos >= absTimes[bpm1]), {


					});
				}); });

				this.redraw;
			});
		};

		this.updateAll;
		this.redraw;

		numPoints.do({ arg i;
			// make a breakpoint
			if((i%pointsPerSegment) == 0, {
				this.setThumbSize(i, 6);

				// color code breakpoints
				if(i.div(pointsPerSegment) == env.releaseNode, {
					this.setFillColor(i, Color.red(0.7));
				},{
					if(i.div(pointsPerSegment) == env.loopNode, {
						this.setFillColor(i, Color.green(0.7));
					},{
						this.setFillColor(i, Color.blue(0.7));
					});
				});

			// Other points should be hidden.
			},{ this.setThumbSize(i, 0) });
		});

	}

	redraw {
		this.value = viewPoints;
	}

	refresh {
		this.updateAll;
		this.value = viewPoints;
	}

	updateAll {
		env.times.size.do({ arg i;
			this.updateSegment(i);
		});
	}

	// updates segment values in viewPoints array
	updateSegment { arg segNum;
		var time, slope, index1, index2, timeOffset;

		// update envelope cache
//		this.debug(env.levels === env.levels);
		env.times = env.times;
//		env.levels = env.levels;
//		env.curves = env.curves;

		segNum = segNum.asInteger;

		time = absTimes[segNum];
		timeOffset = absTimes[0];

		slope = env.times[segNum] / pointsPerSegment;

		index1 = pointsPerSegment * segNum;
		index2 = index1 + pointsPerSegment - 1;

		for(index1, index2, { arg i;
			viewPoints[0][i] = time.linlin(minTime, maxTime, 0.0, 1.0);
			viewPoints[1][i] = env[time - timeOffset].linlin(minLevel, maxLevel, 0.0, 1.0);
			time = time + slope;
		});

		// draw break point at right level
		if(slope == 0, {
			viewPoints[1][index1] = env.levels[segNum].linlin(minLevel, maxLevel, 0.0, 1.0);
		});

		// the last segment has an extra point at the end
		if(segNum == (env.times.size-1), {
			index2 = index2 + 1;
			viewPoints[0][index2] = time.linlin(minTime, maxTime, 0.0, 1.0);
			viewPoints[1][index2] = env.levels.last.linlin(minLevel, maxLevel, 0.0, 1.0);
		});
	}

	minLevel_ { arg level;
		minLevel = level;
		this.refresh;
	}

	maxLevel_ { arg level;
		maxLevel = level;
		this.refresh;
	}

	minTime_ { arg sec;
		minTime = sec;
		this.refresh;
	}

	maxTime_ { arg sec;
		maxTime = sec;
		this.refresh;
	}

	//added by jt ..

	defaultMauseDownAction{|x, y, modifiers, buttonNumber, clickCount|
		var level, time, tbounds;
		tbounds = this.bounds;
		level = y.linlin(tbounds.top, tbounds.top+tbounds.height, maxLevel, minLevel);
		time = x.linlin(tbounds.left, tbounds.left+tbounds.width, minTime, maxTime);
//		this.debug([time, level]);
		if(clickCount > 1){
			//add value
			this.insertAtTime(time, level);
		}
	}

	env_{|e|
		this.initSCEnvelopeEdit(e, pointsPerSegment);
	}

	addBreakPoint{|level|
		var tenv;
		tenv = env.addBreakPoint(level, 0);
		this.initSCEnvelopeEdit(tenv, pointsPerSegment, false);
	}
	insertAtTime{|time, level|
		var tenv;
		tenv = env.insertAtTime(time, level);
		this.initSCEnvelopeEdit(tenv, pointsPerSegment, false);
	}
	clear{
		env = nil; pointsPerSegment=10; viewPoints=0;
		minLevel = maxLevel = minTime = maxTime = totalDurRec = absTimes = numPoints = 0;
		viewPoints = [[],[]];
		this.action_{};
	}

	*paletteExample { arg parent, bounds;
		var v;
		v = this.new(parent, bounds, Env.perc);
		^v
	}
}
