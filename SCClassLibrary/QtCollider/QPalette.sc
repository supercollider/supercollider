QPalette {
  var <>windowColor;
  var <>windowTextColor;
  var <>buttonColor;
  var <>buttonTextColor;
  var <>baseColor;
  var <>baseTextColor;
  var <>highlightColor;
  var <>highlightedTextColor;

  *new {
    // NOTE: Do not init! Color vars that are nil signify widget's default colors.
    ^super.new;
  }

  init {
    windowColor = Color.new;
    windowTextColor = Color.new;
    buttonColor = Color.new;
    buttonTextColor = Color.new;
    baseColor = Color.new;
    baseTextColor = Color.new;
    highlightColor = Color.new;
    highlightedTextColor = Color.new;
  }
}
