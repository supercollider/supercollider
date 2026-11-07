# Version Comments

A version comment sets the language version that code is compiled as. This lets the language change without breaking
existing code: old code keeps its meaning, and new code opts in.

```
//# VERSION legacy
//# VERSION 3.16
```

The release is either `legacy` or `major.minor`. A release before the first one that changed the rules, e.g., `3.13`,
means Legacy. A release after the newest one sclang knows is an error, as is a malformed release.

Code without a version comment is Legacy.

## Files
A version comment on line 1 of a file sets the version of the whole file. This is the same for class files (`.sc`)
and interpreted files (`.scd`). It must be exactly the first line: no blank lines, comments, or spaces before it.

A version comment anywhere else in a file is a warning and is ignored, apart from the per-class version below.

Tools that need a file's version without the compiler, e.g., an editor, read line 1 with this regular expression:

```
^//# VERSION (legacy|[0-9]+\.[0-9]+)[ \t]*\r?$
```

No other manual inspection of sclang code should *ever* be attempted.

The compiler's lexer is the reference. If the two ever disagree, the regular expression is wrong.

## Classes
In a class file, a version comment before anything else in a class's body sets the version of that class only,
instead of the file's.

```
//# VERSION legacy
Foo {
    //# VERSION 3.16
    bar { ^1 }
}
```

An extension (`+ Foo { }`) has its own version, from its own body or its file,
not the version of the class it extends, as an extension is often by another author.

## Interpreted code
When sclang compiles a whole file, e.g., `sclang file.scd`, `load`, `loadRelative`, `executeFile`, or `compileFile`,
the file's line 1 sets its version. 
A string given to `interpret` or `compile` is treated the same way, 
so a version comment on its first line sets its version.

When an editor evaluates part of a file, the part usually doesn't include line 1. 
The editor reads the file's line 1 and sends the version with the code (see below). 
The code is then compiled as that version.

Text with no file, e.g., the REPL or an unsaved document, is Legacy. 
Starting sclang with `--interpreted-version <release>` changes this for text with no file only --- this should only be used in the REPL!

## Help files
A code block in a help file states its version in the tag:

```
code#3.16::
var a = 1;
a.postln;
var b = 2;
::
```

A code block without a version is Legacy. 
The help browser sends the block's version with code evaluated from it.

## Passing a version
Tools and editors pass a version with the code in two ways:
- in sclang, the `version:` keyword of `Interpreter:compile`, `String:interpret`, and the methods like them, e.g.,
  `"var a = 1; a; var b = 2".interpret(version: '3.16')`;
- in sclang's input format, the fourth record of the header, after the file, line, and column.

Here, and only here, the release can also be `latest`, the newest version sclang knows. 
It is for tools and tests that should always use the newest rules. 
It is an error in a version comment, as the code would change meaning on every release.

If code passed with a version also has a version comment on its line 1, and the two disagree, it is an error.
