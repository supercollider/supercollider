// blackrain at realizedsound dot net - 05/2006
//  fix key modidiers bug by Stephan Wittwer 08/2006 - thanks!
//  Knob updates only on value changes - 10/2006
//  GUI.cocoa changes - 04/2007
//
//  03.10.2008 - new implementation:
//    - Knob now is a subclass of SCViewHolder
//    - Relative origin
//
//  01.20.2009 - SCKnob
//    - a subclass of SCUserView again.
//    - isSquare option
//
//  08.03.2010 - QKnob = SCKnob adjusted for GUI.qt scheme (by Jakob Leben)

QKnob : QAbstractStepValue {
  classvar <>defaultMode = \round;

  var <>keystep = 0.01;

  *qtClass {^'QcKnob'}

  *new { arg parent, bounds;
    var me = super.new(parent,bounds);
    me.mode = defaultMode;
    ^me;
  }

  value { ^this.getProperty(\value) }
  value_ { arg val; this.setProperty(\value, val) }
  valueAction_ { arg val; this.value = val; this.doAction }

  mode {
    var m = this.getProperty(\mode);
    ^ #[\round, \horiz, \vert].at(m);
  }

  mode_ { arg inputMode;
    var iMode;
    switch ( inputMode,
      \round, { iMode = 0},
      \horiz, { iMode = 1},
      \vert, { iMode = 2 },
      { ^this }
    );
    this.setProperty( \mode, iMode );
  }

  centered_ { arg bool; this.setProperty( \centered, bool ); }
  centered { ^this.getProperty( \centered ); }

  background { ^this.palette.button; }

  background_ { arg color; this.palette = this.palette.button_(color); }

  // FIXME: find better alternatives to set colors separately.
  color_ { arg colors;
    var p;
    p = this.palette;
    p.button = colors[0];
    p.windowText = colors[1];
    p.window = colors[2];
    p.buttonText = colors[3];
    this.palette = p;
  }

  color {
    var p;
    p = this.palette;
    ^[p.button, p.windowText, p.window, p.buttonText];
  }

  getScale { |modifiers|
    ^case
      { modifiers.isShift } { this.shift_scale }
      { modifiers.isCtrl } { this.ctrl_scale }
      { modifiers.isAlt } { this.alt_scale }
      { 1 };
  }

  defaultKeyDownAction { arg char, modifiers, unicode, keycode, key;
    var zoom = this.getScale(modifiers);

    // standard keydown
    switch( char,
      $r, { this.valueAction = 1.0.rand },
      $n, { this.valueAction = 0.0 },
      $x, { this.valueAction = 1.0 },
      $c, { this.valueAction = 0.5 },

      {
        switch( key,
          16r5b, { this.decrement(zoom) },
          16r5d, { this.increment(zoom) },
          16r1000013, { this.increment(zoom) },
          16r1000014, { this.increment(zoom) },
          16r1000015, { this.decrement(zoom) },
          16r1000012, { this.decrement(zoom) },
          {^this} // propagate on if the key is a no-op
        )
      }
    );
    ^true;
  }

  increment { |zoom=1| ^this.valueAction = (this.value + (keystep * zoom)) }

  decrement { |zoom=1| ^this.valueAction = (this.value - (keystep * zoom)) }

  defaultGetDrag { ^this.value }
  defaultCanReceiveDrag { ^QView.currentDrag.isNumber }
  defaultReceiveDrag { this.valueAction = QView.currentDrag }
}
