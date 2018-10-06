TestPipe : UnitTest {
	var path, command;

	setUp {
		path = PathName.tmp +/+ "sc_testfile.txt";
		command = (linux: "cat", osx: "cat", windows: "type").at(thisProcess.platform.name);
	}

	tearDown {
		File.delete(path);
	}

	test_callSync_normal {
		var file, lines, exitCode;

		file = File(path, "w");
		file << "one line\ntwo lines\n";
		file.close;

		Pipe.callSync(command + path,
			onSuccess: { |contents| lines = contents.postcs },
			onError: { |exit| exitCode = exit }
		);

		// note: if onError is called inappropriately, then 'lines' will be nil
		// and the test fails. So we don't really have to *test* exitCode.
		// we report it for troubleshooting.
		this.assert(lines == "one line\ntwo lines",
			"Pipe.callSync should retrieve the command results exactly, minus trailing newline (exit code was %)".format(exitCode)
		);
	}

	test_callSync_empty {
		var file, lines, exitCode;

		file = File(path, "w");
		file.close;

		Pipe.callSync(command + path,
			onSuccess: { |contents| lines = contents },
			onError: { |exit| exitCode = exit }
		);

		this.assert(exitCode == 0,
			"Pipe.callSync should call onError with exit code 0 for empty results (exit code was %)".format(exitCode)
		);
	}

	test_callSync_error {
		var file, lines, exitCode;

		Pipe.callSync("flibble",
			onSuccess: { |contents| lines = contents },
			onError: { |exit| exitCode = exit }
		);

		this.assert(exitCode != 0,
			"Pipe.callSync with error should give the exit code to the onError function (exit code was %)".format(exitCode)
		);
	}
}
