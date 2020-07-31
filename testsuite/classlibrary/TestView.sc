TestView : UnitTest {
	test_View_settingFontUpdatesVariable {
		var view = View(nil, Rect(100, 100, 100, 100));
		var font = Font.default.pixelSize_(48);
		view.font = font;
		this.assertEquals(view.font, font, "aView.font should answer with the last set font");
		view.close;
	}
}
