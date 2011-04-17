// LISP-like two element cells

Pair : Collection {

	var <>linkDown, <>linkAcross;

	*new { arg linkDown, linkAcross;
		^super.newCopyArgs(linkDown, linkAcross)
	}

	// create from nested array
	*newFrom { arg collection;
		var linkDown = collection.at(0);
		var linkAcross = collection.at(1);
		if(linkDown.isKindOf(Collection)) { linkDown = this.newFrom(linkDown) };
		if(linkAcross.isKindOf(Collection)) { linkAcross = this.newFrom(linkAcross) };
		^this.new(linkDown, linkAcross)
	}


	size { var i = 0, link;
		link = linkAcross;
		while ({ link.respondsTo('linkAcross') },{
			i = i + 1;
			link = link.linkAcross;
		});
		^i
	}
	depth { var i = 0, link;
		link = linkDown;
		while ({ link.respondsTo('linkDown') },{
			i = i + 1;
			link = link.linkDown;
		});
		^i
	}
	do { arg function;
		var i = 0, link, res;
		link = linkAcross;
		while ({ link.respondsTo('linkAcross') },{
			i = i + 1;
			res = function.value(link, i);
			link = link.linkAcross;
		});
		^res
	}

	traverse { arg function;
		// the default traversal order
		^this.depthFirstPreOrderTraversal(function)
	}
	depthFirstPreOrderTraversal { arg function;
		var link;
		function.value(this);
		if ( linkDown.respondsTo('depthFirstPreOrderTraversal'), {
			linkDown.depthFirstPreOrderTraversal(function);
		});
		// iterate linkAcross to conserve stack depth
		link = linkAcross;
		while ({ link.notNil },{
			function.value(link);
			if (link.respondsTo(\linkDown) and:
				{ link.linkDown.respondsTo('depthFirstPreOrderTraversal') }, {
					link.linkDown.depthFirstPreOrderTraversal(function);
			});
			if(link.respondsTo('linkAcross')) {
				link = link.linkAcross;
			} { link = nil };
		});
	}
	depthFirstPostOrderTraversal { arg function;
		var link;
		if ( linkDown.respondsTo('depthFirstPostOrderTraversal'), {
			linkDown.depthFirstPostOrderTraversal(function);
		});
		function.value(this);
		// iterate linkAcross to conserve stack depth
		link = linkAcross;
		while ({ link.notNil },{
			if (link.respondsTo(\linkDown) and:
				{ link.linkDown.respondsTo('depthFirstPostOrderTraversal') }, {
				link.linkDown.depthFirstPostOrderTraversal(function);
			});
			function.value(link);
			if(link.respondsTo('linkAcross')) {
				link = link.linkAcross;
			} { link = nil };
		});
	}

	storeArgs { arg stream;
		^[linkDown, linkAcross]
	}

	printOn { arg stream;
		stream << this.class.name << "(" <<* this.storeArgs << ")"
	}
	storeOn { arg stream;
		stream << this.class.name << "(" <<<* this.storeArgs << ")"
	}
}
