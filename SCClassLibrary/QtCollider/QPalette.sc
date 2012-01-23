QPalette {
  var <window;
  var <windowText;
  var <button;
  var <buttonText;
  var <base;
  var <baseText;
  var <highlight;
  var <highlightText;

  var <hasWindow;
  var <hasWindowText;
  var <hasButton;
  var <hasButtonText;
  var <hasBase;
  var <hasBaseText;
  var <hasHighlight;
  var <hasHighlightText;

  *new {
    // NOTE: Do not init! Color vars that are nil signify widget's default colors.
    ^super.new;
  }

  window_ { arg color; window = color; hasWindow = color.notNil }
  windowText_ { arg color; windowText = color; hasWindowText = color.notNil }
  button_ { arg color; button = color; hasButtonText = color.notNil }
  buttonText_ { arg color; buttonText = color; hasButtonText = color.notNil }
  base_ { arg color; base = color; hasBase = color.notNil }
  baseText_ { arg color; baseText = color; hasBaseText = color.notNil }
  highlight_ { arg color; highlight = color; hasHighlight = color.notNil }
  highlightText_ { arg color; highlightText = color; hasHighlightText = color.notNil }
}
