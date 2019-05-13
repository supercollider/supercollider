Parser {
	var source;
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
		source = sourceString;
		parse = this.prProcessRawParse(rawParse, \meta);
	}

	// PRIVATE
	*prRawParse { |string|
		_ParseExpression
		^nil
	}

	// The parse primitive attaches a \textPosition and \parseLength tag to each parseNode.
	// The \textPosition tag points at the *end* of the parsed element. We derive a \startPosition
	// tag for each element by summing \parseLength values in-order across the parse tree.
	// If the start position and length sum to the end pointer than this node is considered to
	// actually be responsible for a character in the original string, and so this code will
	// add the \substring tag to only those nodes.
	prProcessRawParse { |rawParse, containedBy, startPosition = 0|
		var outputArray = Array.new;
		// rawParse is always pointing at an Array of zero or more Arrays, each one of which
		// should have the \parseNodeProperties prefix, meaning that it is a serialized ParseNode
		// that should be converted into an IdentityDictionary. But we want to process any of the
		// children of each of these and concatenate them into a list, hence the tail recursion
		// to build the tree from the leaves up.
		rawParse.do({ |flatNode, index|
			var whitespace = this.prProcessWhitespace(rawParse, containedBy, startPosition);
			if (whitespace.size > 0, {
				outputArray = outputArray ++ whitespace;
				startPosition = startPosition + outputArray.wrapAt(-1).at(\parseLength);
			});

			if (flatNode.size > 0 and: { flatNode[0] === \parseNodeProperties }, {
				var parseNode = IdentityDictionary.newFrom(flatNode[1..]);
				var containsArray = Array.new;
				parseNode.keysValuesDo({ |key, value|
					// Some key/value pairs will have another array of flatNodes as their children.
					// We process each of these into the containsArray.
					if (value.isArray, {
						containsArray = containsArray ++
						this.prProcessRawParse(value, key, startPosition);
					});
				});
				parseNode.put(\startPosition, startPosition);
				if ((startPosition + parseNode.at(\parseLength)) == parseNode.at(\textPosition), {
					parseNode.put(\substring, source[startPosition..parseNode.at(\parseLength)]);
				});
				startPosition = startPosition + parseNode.at(\parseLength);
				parseNode.put(\contains, containsArray);
				parseNode.put(\containedBy, containedBy);
				outputArray = outputArray.add(parseNode);
			});
		});

		^outputArray;
	}

	prProcessWhitespace { |rawParse, containedBy, startPosition|
		var outputArray = Array.new;
		var whitespaceFound = true;
		while({ whitespaceFound }, {
			var match;
			whitespaceFound = false;
			match = source.findRegexpAt("[ ]+", startPosition);
			if (match.notNil, {
				var node = this.prNewWhitespaceNode(\spaces, startPosition, match, containedBy);
				outputArray = outputArray.add(node);
				startPosition = startPosition + match[1];
				whitespaceFound = true;
			});

			match = source.findRegexpAt("\n+", startPosition);
			if (match.notNil, {
				var node = this.prNewWhitespaceNode(\newLines, startPosition, match, containedBy);
				outputArray = outputArray.add(node);
				startPosition = startPosition + match[1];
				whitespaceFound = true;
			});

			match = source.findRegexpAt("\t+", startPosition);
			if (match.notNil, {
				var node = this.prNewWhitespaceNode(\tabs, startPosition, match, containedBy);
				outputArray = outputArray.add(node);
				startPosition = startPosition + match[1];
				whitespaceFound = true;
			});

			match = source.findRegexpAt("//[^\n]*\n", startPosition);
			if (match.notNil, {
				var node = this.prNewWhitespaceNode(\lineComment, startPosition, match, containedBy);
				outputArray = outputArray.add(node);
				startPosition = startPosition + match[1];
				whitespaceFound = true;
			});

			match = source.findRegexpAt("/[*]([^*]|([*][^/]))*[*]/", startPosition);
			if (match.notNil, {
				var node = this.prNewWhitespaceNode(\blockComment, startPosition, match,
					containedBy);
				outputArray = outputArray.add(node);
				startPosition = startPosition + match[1];
				whitespaceFound = true;
			});
		});

		^outputArray;
	}

	prNewWhitespaceNode { |nodeType, startPosition, match, containedBy|
		var node = IdentityDictionary.new;
		node.put(\nodeType, nodeType);
		node.put(\startPosition, startPosition);
		node.put(\textPosition, startPosition + match[1]);
		node.put(\parseLength, match[1]);
		node.put(\contains, [ ]);
		node.put(\containedBy, containedBy);
		node.put(\substring, match[0]);
		^node;
	}
}