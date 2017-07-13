// TestSCDocHTMLRenderer.sc
// Brian Heim
// 2017-07-12

TestSCDocHTMLRenderer : UnitTest {

	var didSetUp = false;

	setUp {
		if(didSetUp.not) {
			"%: Performing one-time setup to simulate runtime conditions\n".postf(this.class);

			if(SCDoc.didIndexDocuments.not) {
				SCDoc.indexAllDocuments;
			};

			// setup: we need to get the renderer to calculate its 'baseDir' property
			// so that we can safely call htmlForLink
			SCDoc.parseAndRender(SCDoc.documents["Classes/View"]);

			didSetUp = true;
		}
	}

	/*******************************/
	/**** tests for htmlForLink ****/
	/*******************************/

	// external link tests

	test_htmlForLink_externalLink {
		var result = SCDocHTMLRenderer.htmlForLink("http://example.org");
		var expected = "<a href=\"http://example.org\">http://example.org</a>";
		this.assert(result == expected, "Got %, expected %".format(result, expected));
	}

	test_htmlForLink_externalLinkWithLabel {
		var result = SCDocHTMLRenderer.htmlForLink("http://example.org##Example Website");
		var expected = "<a href=\"http://example.org\">Example Website</a>";
		this.assert(result == expected, "Got %, expected %".format(result, expected));
	}

	test_htmlForLink_externalLinkWithAnchor {
		var result = SCDocHTMLRenderer.htmlForLink("http://example.org#Anchor");
		var expected = "<a href=\"http://example.org#Anchor\">http://example.org#Anchor</a>";
		this.assert(result == expected, "Got %, expected %".format(result, expected));
	}

	test_htmlForLink_externalLinkWithAnchorAndLabel {
		var result = SCDocHTMLRenderer.htmlForLink("http://example.org#Anchor#Anchored example");
		var expected = "<a href=\"http://example.org#Anchor\">Anchored example</a>";
		this.assert(result == expected, "Got %, expected %".format(result, expected));
	}

	// inernal link tests

	test_htmlForLink_internalLink {
		var result = SCDocHTMLRenderer.htmlForLink("Classes/View");
		var expected = "<a href=\"./../Classes/View.html\">View</a>";
		this.assert(result == expected, "Got %, expected %".format(result, expected));
	}

	test_htmlForLink_internalLinkWithLabel {
		var result = SCDocHTMLRenderer.htmlForLink("Classes/View##view class");
		var expected = "<a href=\"./../Classes/View.html\">view class</a>";
		this.assert(result == expected, "Got %, expected %".format(result, expected));
	}

	// The link target should have escaped spaces, but the text should not
	test_htmlForLink_internalLinkWithAnchor {
		var result = SCDocHTMLRenderer.htmlForLink("Classes/View#Mouse actions");
		var expected = "<a href=\"./../Classes/View.html#Mouse%20actions\">View: Mouse actions</a>";
		this.assert(result == expected, "Got %, expected %".format(result, expected));
	}

	// The link target should have escaped spaces, but the text should not
	test_htmlForLink_internalLinkWithAnchorAndLabel {
		var result = SCDocHTMLRenderer.htmlForLink("Classes/View#Mouse actions#Hobbledy gobbledy");
		var expected = "<a href=\"./../Classes/View.html#Mouse%20actions\">Hobbledy gobbledy</a>";
		this.assert(result == expected, "Got %, expected %".format(result, expected));
	}

	// empty link
	test_htmlForLink_emptyLink {
		var result = SCDocHTMLRenderer.htmlForLink("");
		var expected = "<a href=\"\">(empty link)</a>";
		this.assert(result == expected, "Got %, expected %".format(result, expected));
	}

	// empty link, with empty anchor and text
	test_htmlForLink_emptyLinkWithAnchorAndText {
		var result = SCDocHTMLRenderer.htmlForLink("##");
		var expected = "<a href=\"\">(empty link)</a>";
		this.assert(result == expected, "Got %, expected %".format(result, expected));
	}

}
