//use for conversion of closed functions in string function

PrePro {
	var <string;
	
	*new { arg string; ^super.newCopyArgs(string.zeroPad) }
	
	findNext { arg from, trueFunc;
		var size, i;
		i = from;
		size = string.size;
		while({
			trueFunc.value(string, i).not
		}, {
			i = i + 1;
			if(i > size, { ^nil });
		})
		^i
	}
	
	findNextClosedFunc { arg from=0;
		^this.findNext(from, { arg string, i; (string[i] == $#) and: { string[i+1] == ${ } }); 
	}
	
	findMatchingBracket { arg from;
		var level=0;
		^this.findNext(from, { arg string, i; 
			var char;
			char = string[i];
			if(char == $}, { 
				level = level - 1 
			}, {
				if(char == ${, {
					level = level + 1
				})
			});
			level < 0
		}); 
	
	}
	
	getClosedFuncIndices {
		var i= -1, end=0, start, res;
		
		while({ end.notNil }, {
			start = this.findNextClosedFunc(i+1);
			if(start.isNil, { ^res });
			end = this.findMatchingBracket(start+3);
			res = res.addAll([start-1, end]);
			i = end
		});
		^res
	}
	
	parseFuncs {
		var array, res, indices, isep, index=0;
		
		indices = this.getClosedFuncIndices;
		isep = indices[0];
		
		array = string.separate({ arg a,b, i;
			var bool;
			bool = ( i == isep );
			if(bool, {
				index = index + 1; 
				isep = indices[index];
			});
			bool;
		});
		res = String.new;
		array.drop(-1).do({ arg item, i;
			res = res ++ item;
			res = res ++ if(i.even, {  "StringFunc(\"" }, { "\")" });
		});
		res = res ++ array.last
		^res
		
	}
	
	*interpret { arg string;
		^this.new(string).parseFuncs.interpret
	}
		
	
}


StringFunc : AbstractFunction {
	var <string, <>func;
	*new { arg string;
		^super.newCopyArgs(string, string.compile.value)
	}
	value { arg ... args;
		^func.valueArray(args)
	}
	valueEnvir { arg ... args;
		^func.valueArrayEnvir(args);
	}
	valueArray { arg args; //revisit
		^func.valueArray(args)
	}
	storeOn { arg stream; stream << string }

}


