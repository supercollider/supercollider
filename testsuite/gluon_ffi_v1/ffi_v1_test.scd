

fork {
try {
	var library_name = thisProcess.argv[0];
	var all_good = false;
	
	var expect = { |action, expected, test_name|
		try {
			var r = action.();
			if (r != expected) {
				var s = "failure: % expect % received %".format(test_name, expected, r);
				s.postln;
				Error(s).throw
			} {
				"success: %".format(test_name).postln
			}
		} { |err|
			err.reportError;
			all_good = false;
		}
	};

	var expect_callback_block = { |action_maker, expected, expectedFromCallback, repetitions, test_name|
		try {
			var c = CondVar();
			var result = Ref(nil);
			
			var r = action_maker.(c, result).();
			
			if (r != expected) {
				var s = "failure: % expect % received %".format(test_name, expected, r);
				s.postln;
				Error(s).throw
			};

            repetitions.do {
                10000.do{ () }; // trigger gc 
                c.waitFor (10) { result.get.notNil };
                10000.do{ () }; // trigger gc 
                if (result.get.isNil) {
                    var s = "failure: % time out".format(test_name);
                    s.postln;
                    Error(s).throw
                };
                result.set(nil);
            }
		} { |err|
			err.reportError;
			all_good = false;
			
		}
	};
	
	var lib_path = File.getcwd +/+ library_name; 


	"LIBRARY PATH = %".format(lib_path).postln;
	
	Gluon.with(lib_path, { |g|
		all_good = true;
		
		expect.({ g.add_f64(1.0, 2.0) }, 3.0, "Basic call back");
		expect.({ g.add_many_f64(1.0, 2.0, -1.0) }, 2.0, "Variable args callback");
		
		expect_callback_block.({| cond_var, result|
            {
                g.with_call_back("hello", callback: { |...args, kwargs|
                    result.set(args ++ kwargs);
                    cond_var.signalAll;
                });
            }
		}, 1, [nil, "hello"], 2, "Callback test");


	});
	
	if(all_good) { 0.exit } { 1.exit }
	
} { |er|
	
	er.reportError;
	1.exit;
	
}
}
