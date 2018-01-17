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

}
