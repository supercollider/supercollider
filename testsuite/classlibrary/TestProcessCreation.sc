TestProcessCreation : UnitTest {
	// ------- String ---------

	test_string_unixCmdGetStdOut {
		this.assert("echo hello world".unixCmdGetStdOut == "hello world\n",
			"calling the program echo with a certain string should return the same string."
		)
	}

	// note: Unix shell doesn't preserve external quotes; Windows shell does.
	// we need to strip newline from unixCmdGetStdOut

	test_string_unixCmdGetStdOut_basicQuotes {
		var str1 = "\"Hello World\"";
		var str2 = "'Hello World'";
		var res1 = if(thisProcess.platform.name == \windows) {str1} {str1.replace("\"", "")};
		var res2 = if(thisProcess.platform.name == \windows) {str2} {str2.replace("\'", "")};
		var out1, out2;
		out1 = "echo %".format(str1).unixCmdGetStdOut.replace("\n", "");
		this.assertEquals(out1, res1, "double quotes result asCompileString: %".format(out1.asCompileString));
		out2 = "echo %".format(str2).unixCmdGetStdOut.replace("\n", "");
		this.assertEquals(out2, res2, "single quotes result asCompileString: %".format(out2.asCompileString));
	}

	test_string_unixCmdGetStdOut_mixingQuotes {
		var str1 = "\"This is 'quoted' inside double quotes\"";
		var str2 = "'This is \"quoted\" inside single quotes'";
		var res1 = if(thisProcess.platform.name == \windows) {str1} {str1.replace("\"", "")};
		var res2 = if(thisProcess.platform.name == \windows) {str2} {str2.replace("\'", "")};
		var out1, out2;
		out1 = "echo %".format(str1).unixCmdGetStdOut.replace("\n", "");
		this.assertEquals(out1, res1, "mixed 'quotes' result asCompileString: %".format(out1.asCompileString));
		out2 = "echo %".format(str2).unixCmdGetStdOut.replace("\n", "");
		this.assertEquals(out2, res2, "single \"quotes\" result asCompileString: %".format(out2.asCompileString));
	}

	// in this test we just check the error code to confirm the command was found
	// on windows we can't use "echo" since it's not a standalone executable
	test_string_unixCmd_quoteCmdAndArg {
		var cond, exitCode;
		var cmd;
		cmd = if(thisProcess.platform.name == \windows) {
			("where".quote + "where".quote)
		} {
			("echo".quote + "hello".quote + "> /dev/null") // need to redirect the output when postOutput is false
		};
		cond = CondVar();
		cmd.unixCmd({|ex| exitCode = ex; cond.signalOne}, false);
		cond.waitFor(2);
		this.assertEquals(exitCode, 0, "quoted command with quoted argument evaluates successfully");
	}

	test_string_unixCmdGet_multipleQuotes {
		var cond, exitCode;
		var str = "\"Hello\" \"World\"";
		var cmd = "echo %".format(str);
		if(thisProcess.platform.name != \windows) {cmd = cmd + "> /dev/null"}; // need to redirect the output when postOutput is false
		cond = CondVar();
		cmd.unixCmd({|ex| exitCode = ex; cond.signalOne}, false);
		cond.waitFor(2);
		this.assertEquals(exitCode, 0, "command with multiple quoted arguments evaluates successfully");
	}

	// ------- SequenceableCollection ---------

	test_collection_unixCmdGetStdOut {
		var cmd = ["echo", "hello", "world"];
		if(thisProcess.platform.name == \windows) {cmd = ["cmd", "/c"] ++ cmd}; // echo is a cmd.exe command on Windows
		this.assertEquals(cmd.unixCmdGetStdOut, "hello world\n",
			"calling the program echo with a certain string should return the same string."
		)
	}

	test_collection_unixCmdGetStdOut_noQuotes {
		var str = "Hello World";
		var res = if(thisProcess.platform.name == \windows) {str.quote} {str}; // argument with space will be quoted on Windows
		var out;
		var cmd = ["echo"];
		if(thisProcess.platform.name == \windows) {cmd = ["cmd", "/c"] ++ cmd};
		out = (cmd ++ [str]).unixCmdGetStdOut.replace("\n", "");
		this.assert(out == res, "arg with spaces asCompileString: %".format(out.asCompileString));
	}

	test_collection_unixCmdGetStdOut_alreadyQuoted {
		var str = "Hello World".quote;
		var res = str; // we should get the quotes back on all platforms
		var out;
		var cmd = ["echo"];
		if(thisProcess.platform.name == \windows) {cmd = ["cmd", "/c"] ++ cmd};
		out = (cmd ++ [str]).unixCmdGetStdOut.replace("\n", "");
		this.assert(out == res, "already quoted arg asCompileString: %".format(out.asCompileString));
	}

	test_collection_unixCmdGetStdOut_alreadyQuotedNoSpaces {
		var str = "Hello".quote;
		var res = str; // we should get the quotes back on all platforms
		var out;
		var cmd = ["echo"];
		if(thisProcess.platform.name == \windows) {cmd = ["cmd", "/c"] ++ cmd};
		out = (cmd ++ [str]).unixCmdGetStdOut.replace("\n", "");
		this.assert(out == res, "already quoted arg without spaces asCompileString: %".format(out.asCompileString));
	}

	test_collection_unixCmdGetStdOut_multipleQuotes {
		var str = "\"Hello\" \"World\"";
		var res = if(thisProcess.platform.name == \windows) {str.asCompileString} {str}; // argument with multiple quotes will be escaped on Windows
		var out;
		var cmd = ["echo"];
		if(thisProcess.platform.name == \windows) {cmd = ["cmd", "/c"] ++ cmd};
		out = (cmd ++ [str]).unixCmdGetStdOut.replace("\n", "");
		this.assert(out == res, "multiple quotes arg asCompileString: %".format(out.asCompileString));
	}

	test_collection_unixCmdGetStdOut_numArgs {
		var cmdArr, args, argsResult, numArgs, resArgs, scriptPath, script, file;
		cmdArr = if(thisProcess.platform.name == \windows) {
			scriptPath = PathName.tmp +/+ "count_args.bat";
			file = File(scriptPath, "w");
			protect {
				file.write(
					"@echo off
for %%A in (%*) do (
set /a count+=1
)
echo %count%
:eof"
				);
			} {
				file.close;
			};
			["cmd", "/c", scriptPath]
		} {
			["sh", "-c", "echo $#", "_"]
		};
		args = ["arg1", "arg two", "arg3"];
		argsResult = (cmdArr ++ args).unixCmdGetStdOut;
		this.assertEquals(argsResult.asInteger, args.size, "number of arguments is correct, even if arguments contain spaces");
		scriptPath !? {File.delete(scriptPath)};
	}

	test_collection_unixCmdGetStdOut_matchArgs {
		var cmdArr, args, argsResult, numArgs, resArgs, scriptPath, file;
		cmdArr = if(thisProcess.platform.name == \windows) {
			scriptPath = PathName.tmp +/+ "echo_args.bat";
			file = File(scriptPath, "w");
			protect {
				file.write(
					"@echo off
for %%A in (%*) do (
echo %%A
)
:eof"
				);
			} {
				file.close;
			};
			["cmd", "/c", scriptPath];
		} {
			["sh", "-c", "for arg in \"$@\"; do echo $arg; done", "_"]
		};
		args = ["arg1", "arg two", "arg3"];
		argsResult = (cmdArr ++ args).unixCmdGetStdOut.split($\n).drop(-1);
		if(thisProcess.platform.name == \windows) {argsResult = argsResult.collect({|thisArg| thisArg.replace("\"", "")})}; // remove quotes from arguments
		this.assertEquals(argsResult, args, "arguments match");
		scriptPath !? {File.delete(scriptPath)};
	}

}
