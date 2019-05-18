Parser {
	var <source;
	var <parse;
	var <rootId;

	var <parseTree;

	*newFromString{ |source|
		var rawParse = Parser.prRawParse(source);
		var result;
		if (rawParse.isNil, {
			result = nil;
		});
		result = super.new.init(source, rawParse);
		^result;
	}

	init { |sourceString, rawParse|
		var active = IdentitySet.new;
		var contained = IdentitySet.new;
		var roots, parseNode, sourcePosition;

		source = sourceString;
		parse = Array.newClear(rawParse.size);

		// First pass: convert rawParse arrays to IdentityDictionaries, and add active nodes only
		// to the node array. Check for edges that would result in nodes being contained by two
		// different owning nodes, and remove those edges. Well-formed node edges will only
		// reference contained nodes with indices lower than their own.
		rawParse.do({ |item, index|
			if (item.notNil, {
				var node = IdentityDictionary.newFrom(item);
				var contains = OrderedIdentitySet.new;

				active.add(index);

				// Build the set of all nodes this node contains.
				this.getContainerLabels(node.at(\nodeType)).do({ |label|
					node.at(label).do({ |containedId|
						if (containedId < index, {
							if (parse[containedId].at(\containedBy).notNil, {
								Error("parser heuristic failed to restore tree shape!").throw;
							});
							contains.add(containedId);
							parse[containedId].put(\containedBy, index);
							contained.add(containedId);
						});
					});
				});

				node.put(\contains, contains);
				parse[index] = node;

				// The highest-index valid node will always be the root.
				rootId = index;
			});
		});

		roots = active.difference(contained);
		if (roots.size != 1 or: { roots.includes(rootId).not }, {
			Error("single tree not detected.").throw;
		});

		// Second pass: there are some nodeTypes, such as \drop, that aid in compilation but don't
		// add any clarity to the parse tree, so we elide them from the tree before finalizing.
		this.prSimplifyTree(rootId);

		parseTree = Array.new(rawParse.size);
		this.prAddNode(\metaRoot);
		parseNode = rootId;
		sourcePosition = 0;

		while (sourcePosition < source.size, {

		});
	}

	// Given the nodeType label, return an in-order Array with all the labels within that node that
	// might contain references to other nodes.
	getContainerLabels { |nodeType|
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

	prSimplifyTree { |nodeId|
		var add = IdentitySet.new;
		var remove = IdentitySet.new;
		parse[nodeId].at(\contains).do({ |containedId|
			this.prSimplifyTree(containedId);

			if (parse[containedId].at(\nodeType) === \drop, {
				add = add.union(parse[containedId].at(\contains));
				remove.add(containedId);
				parse[containedId] = nil;
			});
		});

		parse[nodeId].put(\contains, parse[nodeId].at(\contains).union(add).difference(remove));
		add.do({ |addId|
			parse[addId].put(\containedBy, nodeId);
		});
	}

	prAddNode { |type, containedBy=-1, substring=""|
		var node = IdentityDictionary.new;
		var id = parseTree.size;

		node.add(\type, type);
		node.add(\contains, OrderedIdentitySet.new);
		if (containedBy >= 0, {
			node.add(\containedBy, containedBy);
			parseTree[containedBy].at(\contains).add(id);
		});
		if (substring.size > 0, {
			node.add(\substring, string);
		});
		parseTree = parseTree.add(node);
		^id;
	}

	prParseWhitespace { |sourcePosition, containerId|
		var whitespaceFound = true;
		while({ whitespaceFound }, {
			var match;
			whitespaceFound = false;
			match = source.findRegexpAt("[ ]+", sourcePosition);
			if (match.notNil, {
				this.prAddNode(\spaces, containedBy, match[0]);
				sourcePosition = startPosition + match[1];
				whitespaceFound = true;
			});

			match = source.findRegexpAt("\n+", sourcePosition);
			if (match.notNil, {
				this.prAddNode(\newLines, containedBy, match[0]);
				sourcePosition = sourcePosition + match[1];
				whitespaceFound = true;
			});

			match = source.findRegexpAt("\t+", sourcePosition);
			if (match.notNil, {
				this.prAddNode(\tabs, containedBy, match[0]);
				sourcePosition = sourcePosition + match[1];
				whitespaceFound = true;
			});

			match = source.findRegexpAt("//[^\n]*\n", sourcePosition);
			if (match.notNil, {
				this.prAddNode(\lineComment, containedBy, match[0]);
				sourcePosition = sourcePosition + match[1];
				whitespaceFound = true;
			});

			match = source.findRegexpAt("/[*]([^*]|([*][^/]))*[*]/", sourcePosition);
			if (match.notNil, {
				this.prAddNode(\blockComment, containedBy, match[0]);
				sourcePosition = sourcePosition + match[1];
				whitespaceFound = true;
			});
		});

		^sourcePosition;
	}
}