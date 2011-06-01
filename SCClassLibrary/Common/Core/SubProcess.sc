/* examples:
SubProcess.run { |pid| ("child: got pid"+pid).postln; {1.0.rand} ! 100 } { |res, done, pid| ("parent: child"+pid+"sent"+res).postln }
SubProcess.run { 3.do{|i| SubProcess.send(i)}; (foo:123) } { |res, done, pid| ("parent: child"+pid+"sent"+res+"done:"+done).postln }
*/

SubProcess {
    classvar actions;

    *run { |func, action|
        var pid = this.prBackground(func);
        actions ?? { actions = IdentityDictionary.new };
        actions[pid] = action;
        ^pid;
    }

    *send { |value|
        this.prSendToParent(value.asCompileString);
    }

    *prSendToParent { |value|
        _SendToParent
        ^this.primitiveFailed;
    }

    *prDoAction { |pid, result, done|
        actions[pid].value(result.interpret, done, pid);
        if(done) { actions.removeAt(pid) };
    }

    *prBackground { |func|
        _RunSubProc
        ^this.primitiveFailed;
    }

    *prCallWrapper { |obj, pid|
        ^obj.value(pid).asCompileString;
    }
}

