/*
 * configureTestEnvironment.scd
 *
 * Grabs the UnitTesting Quark and configures the right
 * include/exclude paths to use for testing.
 *
 * Brian Heim, 2017-09-02
 */

try {
	~travisBuildDir = "TRAVIS_BUILD_DIR".getenv;
	~sclangConf = "SCLANG_CONF".getenv;
	Quarks.install("UnitTesting");
	LanguageConfig.addIncludePath(~travisBuildDir +/+ "testsuite/classlibrary");
	LanguageConfig.addExcludePath(~travisBuildDir +/+ "testsuite/classlibrary/server");
	LanguageConfig.addExcludePath(Quarks.folder +/+ "UnitTesting/tests");
	postf("Writing configuration to %\n", ~sclangConf);
	LanguageConfig.store(~sclangConf);
	0.exit;
} { |e|
	e.reportError;
	1.exit;
};
