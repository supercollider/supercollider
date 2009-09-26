/*
// Helper, ClassHelper, UGenHelper, TopicHelper
// (In this file: TopicHelper)
// ________________________________
//
// Classes to auto generate help files following style guides.
// See Helper source for more comments
// started on 04/11/07 - andreavalle
*/

TopicHelper {

	// Topic si the simplest case.
	// Just a replacement in the template

	var <>topic, <>path ;
	classvar text ;

	*new { arg topic, path ;
			^super.new.initTopicHelper( topic, path )
	}

	initTopicHelper { arg aTopic, aPath ;
		topic = aTopic ;
		path = aPath ;
		if ( path.isNil, {
			GUI.current.dialog.savePanel({ arg newPath ;
				 path = newPath ; {this.makeHelp}.defer })
			}, { this.makeHelp }) ;

	}

	createText {
		text = "
<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">
<html>
<head>
<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">
<meta http-equiv=\"Content-Style-Type\" content=\"text/css\">
<title></title>
<meta name=\"Generator\" content=\"Cocoa HTML Writer\">
<meta name=\"CocoaVersion\" content=\"824.41\">
<style type=\"text/css\">
p.p1 {margin: 0.0px 0.0px 0.0px 0.0px; font: 18.0px Helvetica}
p.p2 {margin: 0.0px 0.0px 0.0px 0.0px; font: 12.0px Helvetica; min-height: 14.0px}
p.p3 {margin: 0.0px 0.0px 0.0px 0.0px; font: 12.0px Helvetica}
p.p4 {margin: 0.0px 0.0px 0.0px 0.0px; font: 14.0px Helvetica}
p.p5 {margin: 0.0px 0.0px 0.0px 0.0px; font: 9.0px Monaco; min-height: 12.0px}
p.p6 {margin: 0.0px 0.0px 0.0px 57.0px; text-indent: -57.0px; font: 9.0px Monaco; color: #d40000}
p.p7 {margin: 0.0px 0.0px 0.0px 57.0px; text-indent: -57.0px; font: 9.0px Monaco; min-height: 12.0px}
p.p8 {margin: 0.0px 0.0px 0.0px 57.0px; text-indent: -57.0px; font: 9.0px Monaco}
p.p9 {margin: 0.0px 0.0px 0.0px 0.0px; font: 14.0px Helvetica; min-height: 17.0px}
span.s1 {color: #000000}
span.s2 {color: #1200c4}
span.s3 {color: #d40000}
span.s4 {color: #1200c4}
span.s5 {text-decoration: underline ; color: #1200c4}
span.Apple-tab-span {white-space:pre}
</style>
</head>
<body>
<p class=\"p1\"><b>Some Topic</b></p>
<p class=\"p2\"><br></p>
<p class=\"p3\">Introductory prose discussing Some Topic.</p>
<p class=\"p2\"><br></p>
<p class=\"p4\"><b>Some aspect of Some Topic</b></p>
<p class=\"p2\"><br></p>
<p class=\"p3\">Prose discussion which leads to this example:</p>
<p class=\"p5\"><br></p>
<p class=\"p6\"><span class=\"s1\"><span class=\"Apple-tab-span\">	</span></span>// inline example</p>
<p class=\"p6\"><span class=\"s1\"><span class=\"Apple-tab-span\">	</span>b = </span><span class=\"s2\">SomeClass</span><span class=\"s1\">.new(s, 100, 2); <span class=\"Apple-tab-span\">	</span></span>// explanation of this line</p>
<p class=\"p6\"><span class=\"s1\"><span class=\"Apple-tab-span\">	</span>b.doSomething;<span class=\"Apple-tab-span\">	</span><span class=\"Apple-tab-span\">	</span><span class=\"Apple-tab-span\">	</span><span class=\"Apple-tab-span\">	</span></span>// explanation of this line</p>
<p class=\"p7\"><br></p>
<p class=\"p3\">Further discussion leading to the next bit of code which continues from the previous example:</p>
<p class=\"p5\"><span class=\"Apple-tab-span\">	</span></p>
<p class=\"p8\"><span class=\"Apple-tab-span\">	</span>b.doSomethingMore</p>
<p class=\"p8\"><span class=\"s3\"><span class=\"Apple-tab-span\">	</span></span>b.cleanUp;</p>
<p class=\"p2\"><br></p>
<p class=\"p4\"><b>Some other aspect</b></p>
<p class=\"p2\"><span class=\"Apple-converted-space\"></span></p>
<p class=\"p3\">More prose... Explains how <a href=\"../Core/Object.html\"><span class=\"s4\">Object</span></a> and <a href=\"../Language/Polymorphism.html\"><span class=\"s4\">Polymorphism</span></a> are very important to Some Topic.</p>
<p class=\"p2\"><br></p>
<p class=\"p3\"><b>See also: </b><a href=\"../ServerArchitecture/Buffer.html\"><span class=\"s5\">Buffer</span></a><b> </b>[other related help files]</p>
<p class=\"p2\"><br></p>
<p class=\"p4\"><b>More detailed examples</b></p>
<p class=\"p9\"><br></p>
<p class=\"p6\"><span class=\"s1\">b = </span><span class=\"s2\">SomeClass</span><span class=\"s1\">.new(s, 100, 2); <span class=\"Apple-tab-span\">	</span></span>// explanation of this line</p>
<p class=\"p6\"><span class=\"s1\">b.doSomething;<span class=\"Apple-tab-span\">	</span><span class=\"Apple-tab-span\">	</span><span class=\"Apple-tab-span\">	</span><span class=\"Apple-tab-span\">	</span></span>// explanation of this line</p>
<p class=\"p8\">b.cleanUp</p>
<p class=\"p7\"><br></p>
<p class=\"p8\">...</p>
</body>
</html>
"		.replace("Some Topic", topic) ;  // et voila

	}


	makeHelp {
		// see ClassHelper
		this.createText ;
		File.new(path, "w")
			.write(text)
			.close ;
		// and reopen thru topic.openHelpFile
		// open works if the path is a place where SC looks for Help files
		topic.openHelpFile
	}
}

//"