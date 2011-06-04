/* examples:
SubProcess.run { |pid| ("child: got pid"+pid).postln; {1.0.rand} ! 100 } { |res, done, pid| ("parent: child"+pid+"sent"+res).postln }
SubProcess.run { 3.do{|i| SubProcess.yield(i)}; (foo:123) } { |res, done, pid| ("parent: child"+pid+"sent"+res+"done:"+done).postln }
SubProcess.run { var x = 0; 5.do{|i| x=SubProcess.yield(i+x)}; 0} {|res| ("parent: got"+res).postln; res*10 }
*/

SubProcess {
    classvar actions;

    // run childFunc with arguments (pid) in a forked child process
    // call actionFunc with arguments (result, done, pid) when child yields a value or returns
    // the value returned from actionFunc will be returned from the child yield call
    *run { |childFunc, actionFunc|
        var pid = this.prBackground(childFunc);
        actions ?? { actions = IdentityDictionary.new };
        actions[pid] = actionFunc;
        ^pid;
    }

    // call this from the child to yield a value to the parent
    // this call will block while parents actionFunc is running,
    // returns the value returned by the parents actionFunc.
    *yield { |value|
        ^this.prSendToParent(value.asCompileString).interpret;
    }

// ------------------ private -----------------------

    *prSendToParent { |value|
        _SendToParent
        ^this.primitiveFailed;
    }

    *prDoAction { |pid, result, done|
        var res = actions[pid].value(result.interpret, done, pid);
        if(done) { actions.removeAt(pid) };
        ^res.asCompileString;
    }

    *prBackground { |func|
        _RunSubProc
        ^this.primitiveFailed;
    }

    *prCallWrapper { |obj, pid|
        ^obj.value(pid).asCompileString;
    }
}

