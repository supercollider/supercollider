TestSymbol : UnitTest {

    test_isIdentifier {
        this.assert('x'.isIdentifier, "Single letter is a valid identifier");
        this.assert('itsall'.isIdentifier, "Lowercase word is a valid identifier");
        this.assert('yourFault_'.isIdentifier, "camelCase word with trailing underscore is a valid identifier");
        this.assert('i_screeeeeen'.isIdentifier, "snake_case word is a valid identifier");
        this.assert('my1PhoneCalls8'.isIdentifier, "Valid identifiers can contain digits");

        this.assert(''.isIdentifier.not, "Blank symbol is not a valid identifier");
        this.assert('_noMatter'.isIdentifier.not, "Leading underscore is not a valid identifier");
        this.assert('WhoCalls'.isIdentifier.not, "UpperCamelCase is not a valid identifier");
        this.assert('1iscreeeeeen'.isIdentifier.not, "Leading digit is not a valid identifier");
        this.assert('my phone'.isIdentifier.not, "Valid identifier cannot contain spaces");
        this.assert('calls!'.isIdentifier.not, "Valid identifier cannot contain a symbol");
        this.assert('spiderwébs'.isIdentifier.not, "Valid identifier cannot contain non-ASCII characters");
    }

    test_isBinaryOp {
        this.assert('+'.isBinaryOp, "Single symbol is a valid binary operator");
        this.assert('-*!=!=><@!'.isBinaryOp, "Multiple symbols are a valid binary operator");
        this.assert('-/*-'.isBinaryOp, "A valid binary operator can have '/*' in the middle");

        this.assert(''.isBinaryOp.not, "A blank symbol is not a valid binary operator");
        this.assert('='.isBinaryOp.not, "'=' is not a valid binary operator");
        this.assert('=dimsum'.isBinaryOp.not, "Valid binary operators cannot contain letters");
        this.assert('+3'.isBinaryOp.not, "Valid binary operators cannot contain digits");
        this.assert('/*@@@'.isBinaryOp.not, "Valid binary operators cannot start with '/*'");
        this.assert('//=!'.isBinaryOp.not, "Valid binary operators cannot start with '//'");
        this.assert('€'.isBinaryOp.not, "Valid binary operators cannot contain non-ASCII characters");

    }

}
