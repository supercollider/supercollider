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
// The interpreter sometimes crashes during these tests for me.
// Here is the code to run them:
TestLexerBrutal.new.test_semanticSuffix;
TestLexerBrutal.new.test_blockCommentEncloseSemanticSuffix;
)
