/*

** lock proxymixer to fixed size.
** can move border between nameview and slider to show long proxy names

s.boot;
n = NdefMixer(s);
"bcdefghijk".do { |k| Ndef(k.asSymbol).ar };
"qwrtyu".do { |k| Ndef(k.asSymbol).kr };

n.enableNameResize;

n.getNameLeftBorder;

n.setNameLeftBorder(200);
n.shiftNameLeftBorder(10);
n.shiftNameLeftBorder(-10);

n.shiftArBorder(10);
n.shiftArBorder(-10);

*/
+ ProxyMixer {

	enableNameResize { |flag = true|
		var canDrag = false;

		if (flag) {
			arZone.mouseDownAction = { |arZone, x, y|
				// if between slider and nameView, startDrag
				var nameLeft = this.getNameLeftBorder;
				canDrag = x.absdif(nameLeft) < 15;
			};

			arZone.mouseMoveAction = { |arZone, x, y|
				if (canDrag) { this.setNameLeftBorder(x.postln) };
			};

			arZone.mouseUpAction = { canDrag = false };
		} {
			arZone.mouseDownAction = nil;
			arZone.mouseMoveAction = nil;
			arZone.mouseUpAction = nil;
		}
	}

	makeWindow {
		parent = Window(this.winName,
			bounds.copy.resizeBy(10, 10),
			resizable: false
		).front;
		parent.addFlowLayout;
		hasWindow = true;
	}

	getNameLeftBorder {
		^arGuis[0].nameView.bounds.left;
	}

	setNameLeftBorder { |xpos = 250|
		arGuis.do { |ag|
			var moniZone =  ag.monitorGui.zone;
			var nvbnds = ag.nameView.bounds;
			var nvbndsRight = nvbnds.right;

			moniZone.bounds_(moniZone.bounds.width_(xpos));

			nvbnds = nvbnds.left_(xpos);
			nvbnds = nvbnds.width_(nvbndsRight - xpos);
			ag.nameView.bounds_(nvbnds);
		}
	}

	shiftNameLeftBorder { |x = 0|
		this.setNameLeftBorder(this.getNameLeftBorder + x);
	}

}