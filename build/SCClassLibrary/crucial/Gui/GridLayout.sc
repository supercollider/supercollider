
/*
divide a rect into grids

A1 A2 3 4 5 6 ... 24
B1 B2 B3

request a rect as A1 B2

it can add gutter if you need it

*/


GridLayout {

	var <>bounds,<>numCols,<>numRows,<>gutter;
	var <colWidth,<rowHeight;
	*new { arg bounds,numCols=12,numRows,gutter;
		bounds = if(bounds.isNil,{ GUI.window.screenBounds},{bounds.asRect});
		^super.newCopyArgs(bounds,numCols.asInteger,numRows,gutter ? GUI.skin.margin).init
	}
	/*place { arg view;// view's bounds are expressed in integer quadrants
		var b;
		b = view.bounds;
		view.bounds = this.getBounds( Rect(b.left,b.top)
	}*/
	// a rect encompassing grid a and grid b
	// specify the grids as points eg. 0@0 or 12@2
	calc { |pointA,pointB| // 0@0, 1@1
		var ra,rb;
		ra = this.getRect(pointA.x,pointA.y);
		rb = this.getRect(pointB.x,pointB.y);
		^ra.union(rb)
	}		
	getBounds { | quadrantA,quadrantB| // [0,0], [1,1]
		var ra,rb;
		ra = this.getRect(*quadrantA);
		rb = this.getRect(*quadrantB);
		^ra.union(rb)
	}
	// a single grid rect
	getRect { |row,col| // integers
		^Rect( col * colWidth , row * rowHeight, colWidth,rowHeight)
	}
	mapRect { |integerRect|
		^Rect( integerRect.left * colWidth , integerRect.top * rowHeight,
				integerRect.width * colWidth,integerRect.height * rowHeight)
	}
	init {
		colWidth = bounds.width / numCols;
		rowHeight = colWidth; // bounds.height / numRows;
		// always square, you may exceed the bottom
		if(numRows.isNil,{
			numRows = (bounds.height / rowHeight).floor.asInteger;
		});
	}
	// for visualizing and testing
	gui {
		var w,grids,c;
		var lastPoint;
		w = GUI.window.new("Grid Designer",bounds.resizeBy(10,10));
		c = GUI.compositeView.new(w,bounds);
		c.background = Color.new(1.0, 1.0, 1.0,alpha:0.5);

		grids = numCols.collect({ |ci|
					numRows.collect({ |ri|
						GUI.button.new(c, 
							Rect( ci * colWidth , ri * rowHeight, colWidth,rowHeight))
						.states_([[ri.asString ++ ":"++ci.asString,Color.black,Color.white]])
						.action_({ |butt|  
							if(lastPoint.isNil,{
								lastPoint = ci@ri;
							},{
								this.calc(lastPoint,ci@ri).postln;
								lastPoint = nil;
							});
						})
					})
				});
		w.front;
	}
	
}

