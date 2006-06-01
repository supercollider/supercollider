Pair : Collection { var <>linkDown, <>linkAcross; 
	// LISP-like two element cells
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
			if ( link.linkDown.respondsTo('depthFirstPreOrderTraversal'), {
				link.linkDown.depthFirstPreOrderTraversal(function);
			});
			link = link.linkAcross;
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
			if ( link.linkDown.respondsTo('depthFirstPostOrderTraversal'), {
				link.linkDown.depthFirstPostOrderTraversal(function);
			});
			function.value(link);
			link = link.linkAcross;
		});
	}
	
}

