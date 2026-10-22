// The classes the compiler requires, shared by every test that doesn't start with '// no-prelude'.
// They are compiled, but not included in the output.
AbstractObjectExperimental {}

Object : AbstractObjectExperimental {
    classvar dependantsDictionary, currentEnvironment, topEnvironment, uniqueMethods;
}

Class {}

String {}

True {}

False {}

Nil {}

Float {}

Integer {}

Char {}

Symbol {}

Ref {}

Array {}

Event {}

// Interpreted code is compiled as if it were written inside functionCompileContext, so it can use these members.
Interpreter {
    var <cmdLine, filePath, lineNumber, column, context;
    var <>a, <>b, <>c, <>d, <>e, <>f, <>g, <>h, <>i, <>j;
    var <>k, <>l, <>m, <>n, <>o, <>p, <>q, <>r, <>s, <>t;
    var <>u, <>v, <>w, <>x, <>y, <>z;
    var <>codeDump, <>preProcessor;

    functionCompileContext {}
}
