
QPalette {
  var dataptr, finalizer;

  *new {
    ^super.new.prInit;
  }

  *auto { arg buttonColor, windowColor;
    ^super.new.prAuto(buttonColor, windowColor);
  }

  *system { ^super.new.prSystem }

  *light {
    var p;
    p = QPalette.auto(Color.grey(0.92), Color.grey(0.86));
    p.setColor(Color.grey(0.37), \shadow);
    p.setColor(Color.grey(0.6), \dark);
    p.setColor(Color.grey(0.7), \middark);
    p.highlight = Color(0.3, 0.5, 0.75);
    ^p;
  }

  *dark {
    var p;
    p = QPalette.auto( Color.grey(0.43), Color.grey(0.3) );
    p.base = Color.grey(0.18);
    p.setColor(Color.grey(0.08), \shadow);
    p.highlight = Color(0.25, 0.37, 0.57);
    ^p;
  }

  color { arg role, group;
    ^this.prColor(QColorRole(role), group !? {QColorGroup(group)});
  }

  setColor { arg color, role, group;
    ^this.prSetColor(color, QColorRole(role), group !? {QColorGroup(group)});
  }

  hasColor { arg role, group;
    ^this.prHasColor(QColorRole(role), group !? {QColorGroup(group)});
  }

  window_ { arg color; this.setColor( color, \window ) }
  windowText_ { arg color; this.setColor( color, \windowText ) }
  button_ { arg color; this.setColor( color, \button ) }
  buttonText_ { arg color; this.setColor( color, \buttonText ) }
  base_ { arg color; this.setColor( color, \base ) }
  baseText_ { arg color; this.setColor( color, \baseText ) }
  highlight_ { arg color; this.setColor( color, \highlight ) }
  highlightText_ { arg color; this.setColor( color, \highlightText ) }

  window { ^this.color( \window ) }
  windowText { ^this.color( \windowText ) }
  button { ^this.color( \button ) }
  buttonText { ^this.color( \buttonText ) }
  base { ^this.color( \base ) }
  baseText { ^this.color( \baseText ) }
  highlight { ^this.color( \highlight ) }
  highlightText { ^this.color( \highlightText ) }

  ///// PRIVATE ////

  prInit {
    _QPalette_New
    ^this.primitiveFailed
  }

  prAuto {
    _QPalette_Auto
    ^this.primitiveFailed
  }

  prSystem {
    _QPalette_System
    ^this.primitiveFailed
  }

  prColor { arg role, group;
    _QPalette_Color
    ^this.primitiveFailed
  }

  prSetColor { arg color, role, group;
    _QPalette_SetColor
    ^this.primitiveFailed
  }

  prHasColor { arg role, group;
    _QPalette_HasColor
    ^this.primitiveFailed
  }
}
