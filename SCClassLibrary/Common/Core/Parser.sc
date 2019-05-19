Parser {
	var source;
	var rawParse;

	var <parseTree;

	*newFromString{ |source|
		var rawParse = Parser.prRawParse(source);
		var result;
		if (rawParse.isNil, {
			result = nil;
		});
		result = super.newCopyArgs(source, rawParse).init;
		^result;
	}

	init {
		var pos = 0;

		parseTree = Array.new(rawParse.size);
		this.prAddNode(\meta, -1, "");

		pos = this.prParseWhitespace(pos, 0);
		pos = this.prBuild(rawParse.size - 1, pos, 0);
		pos = this.prParseWhitespace(pos, 0);

		if (pos < source.size, {
			Error("didn't parse entire string!").throw;
		});
	}

	getSource {
		^this.prAppendString(0, "");
	}

	// TO BE DEPRECATED
	// Given the nodeType label, return an in-order Array with all the labels within that node that
	// might contain references to other nodes.
	getRawContainerLabels { |nodeType|
		var labels = switch (nodeType,
			\class, { [ \superClassName, \variables, \methods ] },
			\classExtension, { [ \methods ] },
			\method, { [ \argumentList, \variableList, \body ] },
			\block, { [ \arguments, \variables, \body ] },
			\slot, {  [ ] },
			\variableList, { [ \variableDefinitions ] },
			\variableDefinition, { [ \definitionValue ] },
			\dynamicDictionary, { [ \elements ] },
			\dynamicList, { [ \className, \elements ] },
			\literalList, { [ \className, \elements ] },
			\literalDictionary, { [ \elements ] },
			\staticVariableList, { Error("staticVariableList nodeType not supported").throw },
			\instanceVariableList, { Error("instanceVariableList nodeType not supported").throw },
			\poolVariableList, { Error("poolVariableList nodeType not supported").throw },
			\argumentList, { [ \variableDefinitions, \rest ] },
			\slotDefinition, { Error("slotDefinition nodeType not supported").throw },
			\literal, { [ \literal ] },
			\pushLiteral, { [ \pushLiteral ] },
			\pushName, { [ ] },
			\pushKeyArgument, { [ \selector, \expression ] },
			\call, { [ \arguments, \keyArguments ] },
			\binaryOperationCall, { [ \arguments ] },
			\drop, { [ \firstExpression, \secondExpression ] },
			\assignment, { [ \variableName, \expression ] },
			\multiAssignment, { [ \variables, \expression ] },
			\multiAssignmentVariableList, { [ \variableNames, \rest ] },
			\setter, { [ \selector, \firstExpression, \secondExpression ] },
			\curryArgument, { [ ] },
			\return, { [ \expression ] },
			\blockReturn, { [ ] },
			{ Error("unknown nodeType" + nodeType.asString).throw }
		);
		^labels;
	}

	// PRIVATE
	*prRawParse { |string|
		_ParseExpression
		^nil
	}

	prBuild { |raw, pos, addTo|
		var rawNode;

		// Lazily convert the raw parse nodes into dictionaries, for faster lookup of keys.
		if (rawParse[raw].isArray, {
			rawNode = IdentityDictionary.newFrom(rawParse[raw]);
			rawParse[raw] = rawNode;
		}, {
			rawNode = rawParse[raw];
		});


		// We should now be on a token that is represented in the raw parse tree.
		switch (rawNode.at(\nodeType),
			\block, {
				if (pos < source.size, {
					var block = this.prAddNode(\block, addTo, "(");
					if (source[pos] != $(, {
						Error("parse has block but source has '%'".format(source[pos])).throw;
					});
					pos = this.prParseWhitespace(pos + 1, block);
					if (rawNode.at(\arguments).size > 0, {
						var args = this.prAddNode(\arguments, block, "");
						rawNode.at(\arguments).do({ |argId|
							pos = this.prBuild(argId, pos, args);
							pos = this.prParseWhitespace(pos, args);
						});

						// some kind of argument end token?
						pos = this.prParseWhitespace(pos, block);
					});
					if (rawNode.at(\variables).size > 0, {
						var vars = this.prAddNode(\variables, block, "");
						rawNode.at(\variables).do({ |varId|
							pos = this.prBuild(varId, pos, vars);
							pos = this.prParseWhitespace(pos, vars);
						});

						// some kind of variable end token?
						pos = this.prParseWhitespace(pos, block);
					});

					// Native parse should always have at least a single node within the body,
					// which is the blockReturn node.
					if (rawNode.at(\body).size == 0, {
						Error("native parse has block with no blockReturn in body").throw;
					}, {
						var last = rawNode.at(\body).size - 1;
						if (last > 0, {
							var body = this.prAddNode(\body, block, "");
							last.do({ |i|
								pos = this.prBuild(rawNode.at(\body)[i], pos, body);
								pos = this.prParseWhitespace(pos, body);
							});
						});
						pos = this.prParseWhitespace(pos, block);
						pos = this.prBuild(rawNode.at(\body)[last], pos, block);
					});
				}, {
					// Native parser will build an empty block as root for empty strings.
					if (raw < (rawParse.size - 1) or: { addTo != 0 }, {
						Error("at end of source string but non-empty block in native parse").throw;
					});
					^pos;
				});
			},
			\variableList, {
				var list = this.prAddNode(\variableList, addTo, "var");
				if (source[pos..pos + 2] != "var", {
					Error("parse has variableList but source has '%'".format(
						source[pos..pos + 2])).throw;
				});
				pos = this.prParseWhitespace(pos + 3, list);
				rawNode.at(\variableDefinitions).do({ |def|
					pos = this.prBuild(def, pos, list);
					pos = this.prParseWhitespace(pos, list);
				});
				// Should now be on a semicolon for the end of the variable list.
				if (source[pos] != $;, {
					Error("finished parsing variableList, expecting ; but source has '%'".format(
						source[pos])).throw;
				});
				this.prAddNode(\end, list, ";");
				pos = pos + 1;
			},
			\variableDefinition, {
				var varName = rawNode.at(\name).asString;
				var def = this.prAddNode(\variableDefinition, addTo, varName);
				if (source.containsStringAt(pos, varName).not, {
					Error("parse has variableDefinition '%' but source has '%'".format(
						varName, source[pos..(pos + varName.size)])).throw;
				});
				pos = pos + varName.size;
				if (rawNode.at(\definitionValue).size > 0, {
					var val = this.prAddNode(\value, def, "");
					pos = this.prParseWhitespace(pos, def);
					rawNode.at(\definitionValue).do({ |rawVal|
						pos = this.prBuild(rawVal, pos, val);
						pos = this.prParseWhitespace(pos, val);
					});
				});
			},
			\drop, {
				// Drop nodes are useful for the compiler but are transparent to the parser.
				rawNode.at(\firstExpression).do({ |drop|
					pos = this.prBuild(drop, pos, addTo);
				});
				rawNode.at(\secondExpression).do({ |drop|
					pos = this.prBuild(drop, pos, addTo);
				});
			},
			\dynamicDictionary, {
				// native parser can interpret some empty parenthesis as dynDicts with no
				// elements, in which case we don't add any new parsing nodes.
				if (rawNode.at(\elements).size > 0, {
					Error("can't handly nonempty dyndicts yet").throw;
				});
			},
			\blockReturn, {
				if (source[pos] != $), {
					Error("parse has blockReturn but source has '%'".format(source[pos])).throw;
				});
				if (parseTree[addTo].at(\type) !== \block, {
					Error("parse has blockReturn but adding to '%'".format(
						parseTree[addTo].at(\type).asString)).throw;
				});
				this.prAddNode(\blockReturn, addTo, ")");
				pos = pos + 1;
			},
			{
				Error("can't handle node %, source at '%'".format(rawNode,
					source[pos..min(pos + 10, source.size - 1)])).throw;
			}
		);

		^pos;
	}

	prAddNode { |type, addTo, substr|
		var node = IdentityDictionary.new;
		var id = parseTree.size;

		node.put(\type, type);
		node.put(\contains, OrderedIdentitySet.new);
		if (addTo >= 0, {
			node.put(\containedBy, addTo);
			parseTree[addTo].at(\contains).add(id);
		});
		if (substr.size > 0, {
			node.put(\substring, substr);
		});
		parseTree = parseTree.add(node);
		"adding %: %".format(id, node).postln;
		^id;
	}

	prParseWhitespace { |pos, addTo|
		var whitespaceFound = true;
		while({ whitespaceFound }, {
			var match;
			whitespaceFound = false;
			match = source.findRegexpAt("[ ]+", pos);
			if (match.notNil, {
				this.prAddNode(\spaces, addTo, match[0]);
				pos = pos + match[1];
				whitespaceFound = true;
			});

			match = source.findRegexpAt("[\n]+", pos);
			if (match.notNil, {
				this.prAddNode(\newLines, addTo, match[0]);
				pos = pos + match[1];
				whitespaceFound = true;
			});

			match = source.findRegexpAt("[\t]+", pos);
			if (match.notNil, {
				this.prAddNode(\tabs, addTo, match[0]);
				pos = pos + match[1];
				whitespaceFound = true;
			});

			match = source.findRegexpAt("//[^\n]*\n", pos);
			if (match.notNil, {
				this.prAddNode(\comment, addTo, match[0]);
				pos = pos + match[1];
				whitespaceFound = true;
			});

			match = source.findRegexpAt("/[*]([^*]|([*][^/]))*[*]/", pos);
			if (match.notNil, {
				this.prAddNode(\blockComment, addTo, match[0]);
				pos = pos + match[1];
				whitespaceFound = true;
			});
		});

		^pos;
	}

	prAppendString { |id, str|
		var substring = "";
		if (parseTree[id].at(\substring).notNil, {
			substring = parseTree[id].at(\substring);
		});
		parseTree[id].at(\contains).do({ |cid|
			substring = this.prAppendString(cid, substring);
		});
		^(str ++ substring);
	}
}