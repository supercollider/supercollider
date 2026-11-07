//# VERSION 3.16
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


	/*************************************/
	/**** tests for code tag versions ****/
	/*************************************/

	prRender { |id, version, classCode = false, mark|
		var node = SCDocNode().id_(id).text_("1 + 2");
		version !? { node.children = [SCDocNode().id_(\VERSION).text_(version)] };
		if(classCode) { node.children = node.children.add(SCDocNode().id_(\CLASSCODE)) };
		mark !? { node.children = node.children.add(SCDocNode().id_(mark)) };
		^String.streamContents { |stream| SCDocHTMLRenderer.renderSubTree(stream, node) }
	}

	prLabel { |text|
		^"<div class='codeVersion' title='The language version this code is compiled as'>" ++ text ++ "</div>"
	}

	test_codeVersion_block {
		var html = this.prRender(\CODEBLOCK, "3.16");
		this.assert(html.beginsWith("<div class='codeMirrorContainer' data-sc-version='3.16'>" ++ this.prLabel("3.16") ++ "<textarea"), "a code block writes and shows its version: %".format(html));
	}

	test_codeVersion_blockLegacy {
		var html = this.prRender(\CODEBLOCK, "legacy");
		this.assert(html.beginsWith("<div class='codeMirrorContainer' data-sc-version='legacy'>" ++ this.prLabel("Legacy") ++ "<textarea"), "a legacy code block shows Legacy: %".format(html));
	}

	test_codeVersion_blockWithoutVersion {
		var html = this.prRender(\CODEBLOCK);
		this.assert(html.beginsWith("<div class='codeMirrorContainer'>" ++ this.prLabel("Legacy") ++ "<textarea"), "a code block without a version writes none and shows Legacy: %".format(html));
	}

	test_codeVersion_classBlock {
		var html = this.prRender(\CODEBLOCK, "3.16", true);
		this.assert(html.beginsWith("<div class='codeMirrorContainer' data-sc-version='3.16' data-sc-class-code='true'>" ++ this.prLabel("3.16 class code") ++ "<textarea"), "a class code block writes and shows that it is class code: %".format(html));
	}

	test_codeVersion_classBlockWithoutVersion {
		var html = this.prRender(\CODEBLOCK, nil, true);
		this.assert(html.beginsWith("<div class='codeMirrorContainer' data-sc-class-code='true'>" ++ this.prLabel("Legacy class code") ++ "<textarea"), "a class code block without a version shows Legacy class code: %".format(html));
	}

	test_codeVersion_fragmentBlock {
		var html = this.prRender(\CODEBLOCK, "3.16", false, \FRAGMENTCODE);
		this.assert(html.beginsWith("<div class='codeMirrorContainer' data-sc-version='3.16'>" ++ this.prLabel("3.16 fragment") ++ "<textarea"), "a fragment shows that it is one: %".format(html));
	}

	test_codeVersion_errorBlock {
		var html = this.prRender(\CODEBLOCK, nil, true, \ERRORCODE);
		this.assert(html.beginsWith("<div class='codeMirrorContainer' data-sc-class-code='true'>" ++ this.prLabel("Legacy class code, fails to compile") ++ "<textarea"), "error code shows that it fails to compile: %".format(html));
	}

	test_codeVersion_inline {
		this.assertEquals(this.prRender(\CODE, "legacy"), "<code data-sc-version='legacy'>1 + 2</code>");
		this.assertEquals(this.prRender(\CODE), "<code>1 + 2</code>");
	}

}
