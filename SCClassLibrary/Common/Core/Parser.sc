Parser {
	var <source;
	var <parse;

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
		var roots = IdentitySet.new;
		var keys;

		source = sourceString;
		parse = IdentityDictionary.new;

		// First pass: convert rawParse arrays to maps, and add active nodes only to the node map.
		rawParse.do({ |item, index|
			if (item.notNil, {
				var node = IdentityDictionary.newFrom(item);
				node.put(\containedBy, IdentityDictionary.new);
				parse.put(index, node);
			});
		});

		// Second pass: populate the containedBy dictionary for each node, with key as the id of
		// the containing node and the value as the parent container label.
		parse.keysValuesDo({ |nodeId, node|
			this.getContainerLabels(node.at(\nodeType)).do({ |containerLabel|
				node.at(containerLabel).do({ |containedId|
					parse.at(containedId).at(\containedBy).put(nodeId, containerLabel);
				});
			});
		});

		keys = parse.keys;

		// Third pass: remove some redundant nodes that make the parse directed graph violate the
		// tree property by some contained nodes currently residing in multiple containers. Also
		// identify root nodes as nodes that aren't currently contained in anything.
		keys.do({ |nodeId|
			var node = parse.at(nodeId);
			if (node.notNil, {
				if (node.at(\containedBy).size == 2, {
					// The one case we currently know how to repair reliably is a \variableDefinition
					// nodeType with two \variableList parents, each one of which has the same
					// parent. One will typically have more than other children, and the other will
					// only have this node as a child. We repair this situation by deleting the node
					// with only one child from the tree.
					if (node.at(\nodeType) === \variableDefinition, {
						var redundantId, redundantNode, containerList, containerId, containerKey;
						node.at(\containedBy).keysValuesDo({ |containerId, containerLabel|
							var container = parse.at(containerId);
							if (containerLabel !== \variableDefinitions, {
								Error("unsupported 2-parent containerLabel" +
									containerLabel.asString).throw;
							});
							if (container.at(\nodeType) !== \variableList, {
								Error("unsupported 2-parent container type" +
									container.at(\nodeType).asString).throw;
							});
							if (container.at(containerLabel).size == 1, {
								// Break ties by choosing the lower number, since the parser
								// typically builds from the leaves up a lower number may be
								// closer to tokens in the parse tree.
								if (redundantId.notNil, {
									redundantId = min(redundantId, containerId);
								}, {
									redundantId = containerId;
								});
							});
						});
						// Alright, we've identified the redundant node, remove it from both its
						// container and contained node.
						redundantNode = parse.at(redundantId);
						parse.put(redundantId, nil);
						node.at(\containedBy).put(redundantId, nil);

						containerList = Array.new;
						containerId = redundantNode.at(\containedBy).keys[0];
						containerKey = redundantNode.at(\containedBy).at(containerId);
						node.at(containerId).at(containerKey).do({ |containedId|
							if (containedId != redundantId, {
								containerList = containerList.add(containedId);
							});
						});
						node.at(containerId).put(containerKey, containerList);
					}, {
						Error("unsupported 2-parent nodeType" + node.at(\nodeType).asString).throw;
					});
				}, {
					if (node.at(\containedBy).size == 0, {
						roots.add(nodeId);
					});
				});
			});
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
			\dynamicList, { [ \classname, \elements ] },
			\literalList, { [ \classname, \elements ] },
			\literalDictionary, { [ \elements ] },
			\staticVariableList, { Error("staticVariableList nodeType not supported").throw },
			\instanceVariableList, { Error("instanceVariableList nodeType not supported").throw },
			\poolVariableList, { Error("poolVariableList nodeType not supported").throw },
			\argumentList, { [ \variableDefinitions, \rest ] },
			\slotDefinition, { Error("\slotDefinition nodeType not supported").throw },
			\literal, { [ ] },
			\pushLiteral, { [ ] },
			\pushName, { [ ] },
			\pushKeyArgument, { [ \selector, \expression ] },
			\call, { [ \arguments, \keyArguments ] },
			\binaryOperationCall, { [ \arguments ] },
			\drop, { [ \firstExpression, \secondExpresion ] },
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
}