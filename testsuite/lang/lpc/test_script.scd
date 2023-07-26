// Test script for lexer parser compiler test suite.
// Brian Heim
// 2017-03-04

(
// execute all tests at once
TestLexerBrutal.run;
TestLexerTargetedBrutal.run;
TestParserBrutal.run;
TestCompilerBrutal.run;
)

(
// other options
LPCTestUtils.overwriteFiles = true;                      // default: false
LPCTestUtils.strictOutputChecking = true;                // default: false
LPCTestUtils.doDebug = false;                            // default: true
AbstractLPCBrutalTest.deleteActualFilesOnFinish = false; // default: true
)
