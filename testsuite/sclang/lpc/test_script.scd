// test script for lexer parser compiler test suite
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
LPCTestUtils.strictOutputChecking = true; // default is false
LPCTestUtils.doDebug = false;             // default is true
)
