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

    // Symbols used to be limited to 255 characters.
    longName { |size = 1000|
        ^String.fill(size, { |i| "abcdefghij"[i % 10] })
    }

    test_longSymbol_roundTripsThroughString {
        var name = this.longName;
        var symbol = name.asSymbol;
        this.assertEquals(symbol.asString.size, name.size, "a long symbol keeps every character");
        this.assertEquals(symbol.asString, name, "a long symbol converts back to the same string");
        this.assert(symbol === name.copy.asSymbol, "equal long names give the identical symbol");
        this.assert(symbol !== (name ++ "k").asSymbol, "long names that differ give different symbols");
        this.assert(symbol !== name.keep(999).asSymbol, "a prefix of a long name is a different symbol");
    }

    test_longSymbol_lengthsAroundTheOldLimit {
        [254, 255, 256, 257].do { |size|
            var name = this.longName(size);
            this.assertEquals(name.asSymbol.asString.size, size, "a symbol of % characters keeps them all".format(size));
        }
    }

    test_longSymbol_asCompileString {
        var symbol = this.longName.asSymbol;
        this.assertEquals(symbol.asCompileString, "'" ++ this.longName ++ "'", "asCompileString quotes the whole name");
        this.assert(symbol.asCompileString.interpret === symbol, "asCompileString of a long symbol interprets back to it");
    }

    test_longSymbol_literals {
        var name = this.longName;
        this.assert(("'" ++ name ++ "'").interpret === name.asSymbol, "a quoted long symbol literal compiles");
        this.assert(("\\" ++ name).interpret === name.asSymbol, "a backslash long symbol literal compiles");
        this.assertEquals(("var " ++ name ++ " = 3; " ++ name).interpret, 3, "a long variable name compiles");
    }

    test_longSymbol_setterAndGetter {
        [255, 1000].do { |size|
            var symbol = this.longName(size).asSymbol;
            var setter = symbol.asSetter;
            this.assertEquals(setter.asString.size, size + 1, "asSetter appends '_' to a % character symbol".format(size));
            this.assert(setter.isSetter, "asSetter gives a setter");
            this.assert(setter.asGetter === symbol, "asGetter removes the '_' again");
        }
    }

    test_longSymbol_setterSyntax {
        var name = this.longName;
        var code = "var e = (); e.% = 5; e.%".format(name, name);
        this.assertEquals(code.interpret, 5, "'x.name = value' works with a long name");
    }

    test_stringAsSymbol_stopsAtNull {
        var string = String.with($a, $b, 0.asAscii, $c, $d);
        this.assert(string.asSymbol === 'ab', "a symbol ends at the first null character");
    }

    test_archive_longSymbol {
        var symbol = this.longName.asSymbol;
        var object = [symbol, 'short', [symbol]];
        var result = object.asBinaryArchive.unarchive;
        this.assertEquals(result, object, "an array holding long symbols survives a binary archive");
        this.assert(result[0] === symbol, "the unarchived long symbol is the identical symbol");
        this.assert(symbol.asBinaryArchive.unarchive === symbol, "a long symbol on its own survives a binary archive");
    }

    test_archive_readsVersion2 {
        // '!SCa', version 2, no objects, then the symbol 'abc' with a one byte length.
        var archive = Int8Array[33, 83, 67, 97, 0, 0, 0, 2, 0, 0, 0, 0, 115, 3, 97, 98, 99];
        this.assert(archive.unarchive === 'abc', "a version 2 archive with a one byte symbol length still reads");
    }

}
