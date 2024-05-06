TestScale : UnitTest {
    test_newFromKey {
		this.assertEquals(Scale.at(\major), Scale.major, "Scale-doesNotUnderstandAbout creates valid scale.");
		this.assertEquals(Scale.at(\major), Scale.major(tuning: Tuning.et(12)), "Scale-doesNotUnderstandAbout creates valid scale.");
    }
}