TestMain : UnitTest {
	test_versionAtMost_equal_returnsTrue {
		this.assert(Main.versionAtMost(Main.scVersionMajor, Main.scVersionMinor, Main.scVersionPatch))
	}

	test_versionAtMost_equalWithNilPatch_returnsTrue {
		this.assert(Main.versionAtMost(Main.scVersionMajor, Main.scVersionMinor, nil))
	}

	test_versionAtMost_equalWithNilMinorAndPatch_returnsTrue {
		this.assert(Main.versionAtMost(Main.scVersionMajor, nil, nil))
	}

	test_versionAtMost_majorGreater_returnsTrue {
		this.assert(Main.versionAtMost(Main.scVersionMajor + 1, nil, nil))
	}

	test_versionAtMost_majorLess_returnsFalse {
		this.assert(Main.versionAtMost(Main.scVersionMajor - 1, nil, nil).not)
	}

	test_versionAtMost_patchGreater_returnsTrue {
		this.assert(Main.versionAtMost(Main.scVersionMajor, Main.scVersionMinor, Main.scVersionPatch + 1))
	}

	test_versionAtMost_patchLess_returnsFalse {
		this.assert(Main.versionAtMost(Main.scVersionMajor, Main.scVersionMinor, Main.scVersionPatch - 1).not)
	}

	test_versionAtLeast_equal_returnsTrue {
		this.assert(Main.versionAtLeast(Main.scVersionMajor, Main.scVersionMinor, Main.scVersionPatch))
	}

	test_versionAtLeast_equalWithNilPatch_returnsTrue {
		this.assert(Main.versionAtLeast(Main.scVersionMajor, Main.scVersionMinor, nil))
	}

	test_versionAtLeast_equalWithNilMinorAndPatch_returnsTrue {
		this.assert(Main.versionAtLeast(Main.scVersionMajor, nil, nil))
	}

	test_versionAtLeast_majorGreater_returnsFalse {
		this.assert(Main.versionAtLeast(Main.scVersionMajor + 1, nil, nil).not)
	}

	test_versionAtLeast_majorLess_returnsTrue {
		this.assert(Main.versionAtLeast(Main.scVersionMajor - 1, nil, nil))
	}

	test_versionAtLeast_patchGreater_returnsFalse {
		this.assert(Main.versionAtLeast(Main.scVersionMajor, Main.scVersionMinor, Main.scVersionPatch + 1).not)
	}

	test_versionAtLeast_patchLess_returnsTrue {
		this.assert(Main.versionAtLeast(Main.scVersionMajor, Main.scVersionMinor, Main.scVersionPatch - 1))
	}
}
