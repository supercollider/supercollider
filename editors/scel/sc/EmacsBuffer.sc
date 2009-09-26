// Emacs Widget library bindings for SCLang

EmacsBuffer { // Represents an Emacs buffer
	classvar all;
	var <name, <>onClose, <keymap;
	*initClass {
		Class.initClassTree(Emacs);
		all = Dictionary.new;
		Emacs.evalLispExpression(
			[\require, [\quote, 'wid-edit']].asLispString,
			{Emacs.evalLispExpression(
				[\defvar, 'sclang-widgets', \nil,
					"Maps IDs to Emacs widget objects."]
				.asLispString,
				{Emacs.evalLispExpression(
					['make-variable-buffer-local', [\quote, 'sclang-widgets']]
					.asLispString)})});
	}
	*new {|name="*SCWindow*"|
		^super.newCopyArgs(name).init;
	}
	init {
		all[name] = this;
		keymap = Dictionary.new;
		Emacs.evalLispExpression(
			['with-current-buffer', ['get-buffer-create', name],
				[\let, [['inhibit-read-only', \t]], ['erase-buffer']],
				['use-local-map',
					[\let, [[\map, ['make-sparse-keymap']]],
						['set-keymap-parent', \map, 'widget-keymap'],
						\map]],
				['add-hook', [\quote, 'kill-buffer-hook'],
					[\lambda, [],
						['sclang-eval-string',
							['sclang-format', "EmacsBuffer.killed(%o)",
								['buffer-name']]]],
					\nil, \t]].asLispString)
	}
	*at {|name| ^all[name] }
	defineKey {|keySeq,func|
		keymap.put(keySeq, func);
		Emacs.evalLispExpression(
			this.use(['local-set-key', keySeq,
				[\lambda, [],
					[\interactive],
					['sclang-eval-string',
						['sclang-format', "EmacsBuffer.at(%o).keymap.at(%o).value",
							['buffer-name'], keySeq]]]]).asLispString)
	}
	front { Emacs.evalLispExpression(['switch-to-buffer', name].asLispString) }
	use {|...args| ^['with-current-buffer', ['get-buffer', name]]++args }
	insert {|string|
		Emacs.evalLispExpression(
			this.use(['widget-insert', string]).asLispString
		)
	}
	goto {|position|
		Emacs.evalLispExpression(
			this.use(['goto-char', position]).asLispString
		)
	}
	gotoBob {
		Emacs.evalLispExpression(
			this.use(['goto-char', ['point-min']]).asLispString
		)
	}
	gotoEob {
		Emacs.evalLispExpression(
			this.use(['goto-char', ['point-max']]).asLispString
		)
	}
	newline { this.insert("\n")	}
	editableField {|tag, value, action|
		EmacsEditableField(this, tag, value).action=action
	}
	button {|states, action, prefix="[", suffix="]"|
		EmacsButton(this, states, action, prefix, suffix)
	}
	closeButton {
		EmacsPushButton(this, "Close").action={this.free}
	}
	*killed {|name|
		var buf;
		buf = this.at(name);
		if (buf.isNil.not) {
			buf.onClose.value(buf);
			all.removeAt(name);
		}
	}
	free {
		onClose.value(this);
		all.removeAt(name);
		Emacs.evalLispExpression(['kill-buffer', name].asLispString,
			{super.free})
	}
}

EmacsWidget {
	classvar allocator, <idmap;
	var <buffer, <type, <args, <id, <enabled;
	*initClass {
		allocator = StackNumberAllocator(0,1024);
		idmap = Array.newClear(1024);
	}
	*new {|buffer, type ... args|
		^super.newCopyArgs(buffer,type,args).init;
	}
	init {
		id = allocator.alloc;
		idmap[id] = this;
		enabled = true;
		Emacs.evalLispExpression(
			buffer.use(
				['add-to-list', [\quote, 'sclang-widgets'],
					[\cons, id, ['widget-create', [\quote, type], ':id', id]++args]],
				['widget-setup']).asLispString)
	}
	wPut {|argKey, argValue, handler|
		Emacs.evalLispExpression(
			buffer.use(['widget-put',
				[\cdr, [\find, id, 'sclang-widgets', ':key', [\quote, \car]]],
				(':'++argKey).asSymbol,
				[\quote, argValue]]).asLispString, handler)
	}
	wValueSet {|argValue, handler|
		Emacs.evalLispExpression(
			buffer.use(['widget-value-set',
				[\cdr, [\find, id, 'sclang-widgets', ':key', [\quote, \car]]],
				argValue]).asLispString, handler)
	}
	enabled_ {|argValue, handler|
		if (argValue) {
			Emacs.evalLispExpression(
				buffer.use(['widget-apply',
					[\cdr, [\find, id, 'sclang-widgets', ':key', [\quote, \car]]],
					':activate'], \nil).asLispString, {enabled=true;handler.value})
		} {
			Emacs.evalLispExpression(
				buffer.use(['widget-apply',
					[\cdr, [\find, id, 'sclang-widgets', ':key', [\quote, \car]]],
					':deactivate'], \nil).asLispString, {enabled=false;handler.value})
		}
	}
}

EmacsPushButton : EmacsWidget {
	var <>action;
	*new {|buffer, tag|
		^super.new(buffer, 'push-button', ':tag', tag, ':action',
			[\lambda, [\widget, \event],
				['sclang-eval-string',
					['sclang-format', "EmacsWidget.idmap[%o].action.value",
						['widget-get', \widget, ':id']]]])
	}
}

EmacsEditableField : EmacsWidget {
	var <>action;
	*new {|buffer, tag, value=""|
		^super.new(buffer, 'editable-field', ':tag', tag,
			':format', "%{%t%}: %v",
			':action',
			[\lambda, [\widget, \event],
				['sclang-eval-string',
					['sclang-format', "EmacsWidget.idmap[%o].action.value(%o)",
						['widget-get', \widget, ':id'],
						['widget-value', \widget]]]],
		value)
	}
}

EmacsNumber : EmacsWidget {
	var <>action, <>spec, <value;
	*new {|buffer, tag, spec, action, value=0,size=10|
		^super.new(buffer, \number, ':tag', tag,
			':format', "%{%t%}: %v",
			':size', size?\nil,
			':min', spec.minval,
			':max', spec.maxval,
			':action',
			[\lambda, [\widget, \event],
				[\let, [[\val, ['widget-value', \widget]]],
					[\cond,
						[['>', \val, ['widget-get', \widget, ':max']],
							[\error, "Too much"]],
						[['<', \val, ['widget-get', \widget, ':min']],
							[\error, "Too less"]],
						[\t,
							['sclang-eval-string',
								['sclang-format', "EmacsWidget.idmap[%o].valueFromEmacs(%o)",
									['widget-get', \widget, ':id'], \val]]]]]],
			value).action_(action).spec_(spec).initValue(value)
	}
	valueFromEmacs {|argValue|
		value = argValue;
		action.value(value)
	}
	value_ {|argValue|
		Emacs.evalLispExpression(
			buffer.use(
				['save-excursion',
					['widget-value-set', [\cdr, [\find, id, 'sclang-widgets', ':key', [\quote, \car]]], argValue],
					['widget-setup']],
				argValue).asLispString,
			{|result|value=result})
	}
	initValue {|argValue|value=argValue}
}

EmacsButton : EmacsWidget {
	var <>action, <value, <states;
	*new {|buffer, states=#[], action, prefix="[", suffix="]"|
		^super.new(buffer, 'sclang-button',
			':button-prefix', prefix,
			':button-suffix', suffix,
			':states', [\quote, states],
			0).action_(action).initValue(0).initStates(states)
	}
	valueFromEmacs {|argValue|
		value = argValue;
		action.value(value)
	}
	value_ {|argValue|
		this.wValueSet(argValue, {value = argValue});
	}
	states_ {|argStates|
		states=argStates;
		Emacs.sendToLisp(\_widgetSetStates, [buffer.name, id, states, value]);
		/*		Emacs.evalLispExpression(
			buffer.use(
				[\let, [[\widget, [\cdr, [\find, id, 'sclang-widgets', ':key', [\quote, \car]]]],
					[\states, [\quote, states]]],
					['widget-put', \widget, ':states', \states],
					['widget-value-set', \widget, [\nth, value, \states]]]
			).asLispString) */
	}
	initStates {|argStates|states=argStates}
	initValue {|argValue|value=argValue}
}

EmacsText : EmacsWidget {
	var <string, <size, <align;
	*new {|buffer, string="", size, align=\center|
		^super.new(buffer, \item, ':format', "%v", ':size', size?\nil, ':align', [\quote, align],
			':value-create',
			[\lambda, [\widget],
				['let*', [
					[\align, [\or, ['widget-get', \widget, ':align'], [\quote, \center]]],
					[\string, ['widget-get', \widget, ':value']],
					[\size, [\cond, [[\null, ['widget-get', \widget, ':size']], [\length, \string]],
						[['<', ['widget-get', \widget, ':size'], [\length, \string]], [\length, \string]],
						[\t, ['widget-get', \widget, ':size']]]]
				],
					[\insert,
						[\cond,
							[['=', \size, [\length, \string]], \string],
							[['>', \size, [\length, \string]],
								[\cond,
									[[\eq, \align, [\quote, \left]],
										[\concat, \string, ['make-string', ['-', \size, [\length, \string]], $ ]]],
									[[\eq, \align, [\quote, \right]],
										[\concat, ['make-string', ['-', \size, [\length, \string]], $ ], \string]],
									[[\eq, \align, [\quote, \center]],
										[\let, [[\half, ['/', ['-', \size, [\length, \string]], 2]]],
											[\if, ['=', ['+', [\length, \string], ['*', \half, 2]], \size],
												[\concat, ['make-string', \half, $ ], \string, ['make-string', \half, $ ]],
												[\concat, ['make-string', \half, $ ], \string, ['make-string', \half, $ ], " "]]]]]]]]]],
			string).initValue(size?string.size,align,string)
	}
	string_ {|argString|
		string = argString.asString;
		this.wValueSet(string);
	}
	size_ {|argSize|
		this.wPut(\size, size=argSize, {this.wValueSet(string)})
	}
	align_ {|argAlign|
		this.wPut(\align, align=argAlign, {this.wValueSet(string)})
	}
	initValue {|argSize,argAlign,argValue|size=argSize;align=argAlign;string=argValue}
}

EmacsClassTree : EmacsWidget {
	*new {|buffer, className, open=true|
		^super.new(buffer, 'sclang-class-tree', ':tag', className.asString,
			':open', if(open){\t}{\nil},
			':node', [\quote, ['sclang-file-position',
				':filename', className.asSymbol.asClass.filenameSymbol.asString,
				':char-pos', className.asSymbol.asClass.charPos+1,
				':tag', className.asString]])
	}
	*dynargs {|className|
		var class;
		class = className.asSymbol.asClass;
		^(class.subclasses.asArray.copy.sort{|a,b|a.name <= b.name}.collect{|sc|
			['sclang-class-tree', ':tag', sc.name.asString,
				':node', ['sclang-file-position',
					':filename', sc.filenameSymbol.asString,
					':char-pos', sc.charPos+1,
					':tag', sc.name.asString]]})
		++
		(class.class.methods.asArray.copy.sort({|a,b| a.name <= b.name }).collect{|m|
			['sclang-file-position',
				':filename', m.filenameSymbol.asString,
				':char-pos', m.charPos+1,
				':tag', "*"++m.name.asString]})
		++
		(class.methods.asArray.copy.sort({|a,b| a.name <= b.name }).collect{|m|
			['sclang-file-position',
				':filename', m.filenameSymbol.asString,
				':char-pos', m.charPos+1,
				':tag', m.name.asString]})
	}
}

EmacsClassBrowser {
	var w, classTree;
	*new {|class|
		^super.newCopyArgs(EmacsBuffer("*"++class.asClass.name++"*")).init(class);
	}
	init {|class|
		classTree=EmacsClassTree(w, class);
		w.gotoBob.front;
	}
}
