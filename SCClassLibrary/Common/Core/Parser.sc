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
		source = sourceString;
		parse = Array.newClear(rawParse.size);
		rawParse.do({ |item, index|
			if (item.notNil, {
				var node = IdentityDictionary.newFrom(item);
				var contains = IdentitySet.new;
				node.keysValuesDo({ |key, value|
					if (value.isArray and: { value.isString.not }, {
						value.do({ |containedItem|
							contains.add(containedItem);
						});
					});
				});
				node.put(\contains, contains);
				node.put(\containedBy, IdentitySet.new);
				parse[index] = node;
			});
		});
		parse.do({ |item, index|
			if (item.notNil, {
				item.at(\contains).do({ | containedItem |
					parse[containedItem].at(\containedBy).add(index);
				});
			});
		});
	}

	// PRIVATE
	*prRawParse { |string|
		_ParseExpression
		^nil
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