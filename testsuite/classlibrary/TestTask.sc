TestTask : UnitTest {

	test_stop_internally {
		var running, stopped;
		var task = Task {
			running = true;
			0.001.wait;
			stopped = true;
			task.stop;
			stopped = false;
		};
		task.play;
		0.002.wait;
		this.assert(stopped, "Task should stop itself immediately from within using the stop message");

	}

	test_stop_without_running {
		var task = Task { 0.001.wait; task.stop; };
		task.play;
		0.002.wait;
		task.stop;
		this.assert(task.isPlaying.not, "Task be stoppable even if not running");
	}

	test_play_with_SystemClock {
		this.makePlayTestWithClock(SystemClock, "SystemClock");
	}

	test_play_with_AppClock {
		this.makePlayTestWithClock(AppClock, "AppClock");
	}

	test_play_with_TempoClock {
		this.makePlayTestWithClock(TempoClock.default, "TempoClock.default");
	}

	test_play_with_LinkClock {
		this.makePlayTestWithClock(LinkClock.new, "LinkClock.new");
	}

	makePlayTestWithClock { |clock, nameString|
		var ok = false;
		var task = Task { 0.001.wait; ok = true; cond.signalAll };
		var cond = CondVar.new;
		task.play(clock);
		cond.waitFor(1, { ok });
		task.stop;
		clock.stop;
		this.assert(ok, "Task plays with " ++ nameString);
	}

}
