QuarkSVNRepository
{
	var <url, cache;
	classvar nodeFields;

	*initClass {
		nodeFields = Dictionary[
			"URL: " -> \url,
			"Node Kind: " -> \type,
			"Repository Root: " -> \repos,
			"Repository UUID: " -> \uuid,
			"Revision: " -> \revision,
			"Last Changed Date: " -> \date
		]
	}

	*new { | url ="https://svn.sourceforge.net/svnroot/quarks"|
		^this.newCopyArgs(url)
	}

	makeNode { | lines |
		var node = ();
		lines.do { |line|
			nodeFields.keysValuesDo { |prefix,key|
				if (line.beginsWith(prefix)) {
					node.put(key, line.copyToEnd(prefix.size));
				}
			}
		};
		^node
	}

	/* nodes {
		var urls, info;
		urls = this.svn("propget", prop, "--recursive", url)
		.collect { |line| $" ++ line.split($-)[0].copyFromEnd(1).escapeChar($") ++ $" }; // hmm
		info = this.svn("info", *urls).delimit(_.isEmpty);
		if (info.last.isEmpty) { info.pop };
		^info.collect(this.makeNode(_)).reject(_.isNil)
	} */
	
	quarkURLs {
		// match all QUARK in ls
		var all;
		all = this.svn("ls","--recursive",url);
		all.debug;
		^all
		// and cache it till next recompile
	}

	quarks {
		var url, info, node, q;
		^this.quarkURLs.collect { |url|
			info = this.svn("info", url);
			if (info.first.beginsWith(url ++ "  (Not a valid URL)").not) {
				node = this.makeNode(info);
				if (node.type == "file") {
					q = Quark.fromString(this.svn("cat", node.url).join(Char.nl) );
					q.path = url.copyToEnd(this.url.size);
					q
				};
			};
		}.reject(_.isNil)
	}
	findQuark { arg name, version;
		var matches;
		matches = this.quarks.collect({ |q| q.name == name });
		if(version.notNil,{
			matches = matches.select({ |q| q.version >= version });
		});
		^matches.sort({ |a,b| a.version > b.version }).first
	}
	checkout { | q, localRoot |
		// may have to create not yet existing intermediate directories
		this.svn("co", this.url ++ q.path, localRoot ++  q.path).join(Char.nl).postln;
	}

	svn { | cmd ... args |
		var pipe, line, contents = List.new;
		pipe = Pipe( ("svn" + cmd + args.join(" ")).debug, "r");
		while { line = pipe.getLine; line.notNil } {
			line.debug("adding line");
			contents.add(line);
		};
		"exiting with contents".debug;
		^contents
	}
}
