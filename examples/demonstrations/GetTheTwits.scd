// =====================================================================
// Twitter is popular amongst some SC users.
// This is a script to retrieve all the supercollider twits
// Original code by Eric Skogen, with some tweaks by Marije Baalman (nescivi)
// =====================================================================

// NOTE: to use this script you need to have installed the Quark: XML
// See the Quarks helpfile on how to install this Quark.
// You also need to have "curl" installed on your system.

(
var p, a, l, d, doc;
p = Pipe.new(
  "curl http://search.twitter.com/search.atom?q=%23supercollider+play",
  "r"
);
a = "";
l = p.getLine;
while({l.notNil}, {a = a + l; l = p.getLine; });
p.close;

d = DOMDocument.new;
d.parseXML(a);
t = d.getElementsByTagName("entry").collect { |e|

  "// by " + e.getElementsByTagName("author").first
          .getElementsByTagName("name").first.getText() + ":\n" +

  e.getElementsByTagName("title").first.getText()

};

Document( "supercollider tweets", "// #supercollider tweets\n\n\n" +
  t.join("\n\n\n\n")).syntaxColorize;
)



// THE CAVEAT SLIDE
// a slight delay is to be expected, to await the gathering of twitters' reply.
// (do not be alarmed. all part of the show, my friends)
// oh and yeah you'll need the XML Quark
// (http://quarks.svn.sourceforge.net/viewvc/quarks/XML/)
// and it only works if you can run Pipe
