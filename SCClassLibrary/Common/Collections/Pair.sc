// LISP-like two element cells

Pair : Collection {

	var <>linkDown, <>linkAcross;

	*new { |linkDown, linkAcross|
		^super.newCopyArgs(linkDown, linkAcross)
	}

	// create from nested array
	*newFrom { |collection|
		var linkDown = collection.at(0);
		var linkAcross = collection.at(1);
		if(linkDown.isKindOf(Collection)) { linkDown = this.newFrom(linkDown) };
		if(linkAcross.isKindOf(Collection)) { linkAcross = this.newFrom(linkAcross) };
		^this.new(linkDown, linkAcross)
	}


	size {
		var i = 0, link;
		link = linkAcross;
		while { link.respondsTo('linkAcross') } {
			i = i + 1;
			link = link.linkAcross
		};
		^i
	}
	depth {
		var i = 0, link;
		link = linkDown;
		while { link.respondsTo('linkDown') } {
			i = i + 1;
			link = link.linkDown
		};
		^i
	}
	do { |function|
		var i = 0, link, res;
		link = linkAcross;
		while { link.respondsTo('linkAcross') } {
			i = i + 1;
			res = function.value(link, i);
			link = link.linkAcross
		};
		^res
	}

	remove { ^this.shouldNotImplement(thisMethod) }
	add { ^this.shouldNotImplement(thisMethod) }

	traverse { |function|
		// the default traversal order
		^this.depthFirstPreOrderTraversal(function)
	}
	depthFirstPreOrderTraversal { |function|
		var link;
		function.value(this);
		if(linkDown.respondsTo('depthFirstPreOrderTraversal')) {
			linkDown.depthFirstPreOrderTraversal(function)
		};
		// iterate linkAcross to conserve stack depth
		link = linkAcross;
		while { link.notNil } {
			function.value(link);
			if(link.respondsTo(\linkDown) and:
				{ link.linkDown.respondsTo('depthFirstPreOrderTraversal') }) {
					link.linkDown.depthFirstPreOrderTraversal(function)
				};
			if(link.respondsTo('linkAcross')) {
				link = link.linkAcross
			} { link = nil }
		}
	}
	depthFirstPostOrderTraversal { |function|
		var link;
		if(linkDown.respondsTo('depthFirstPostOrderTraversal')) {
			linkDown.depthFirstPostOrderTraversal(function)
		};
		function.value(this);
		// iterate linkAcross to conserve stack depth
		link = linkAcross;
		while { link.notNil } {
			if(link.respondsTo(\linkDown) and:
				{ link.linkDown.respondsTo('depthFirstPostOrderTraversal') }) {
					link.linkDown.depthFirstPostOrderTraversal(function)
				};
			function.value(link);
			if(link.respondsTo('linkAcross')) {
				link = link.linkAcross
			} { link = nil }
		}
	}

	storeArgs { |stream|
		^[linkDown, linkAcross]
	}

	printOn { |stream|
		stream << this.class.name << "(" <<* this.storeArgs << ")"
	}
	storeOn { |stream|
		stream << this.class.name << "(" <<<* this.storeArgs << ")"
	}
}
