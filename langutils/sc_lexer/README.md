# sc_lexer

Turns normalised text into tokens, each with a type and a location. It is header-only apart from two small files, and it
depends on [sc_source](../sc_source/README.md). Everything is in namespace `sc::lex`.


Produces a flat output without semantics from the language.

Is designed to not depend on the language version, so care should be taken to avoid this is possible.

## What is in it

| Header | Purpose |
|---|---|
| `tokens.hpp` | `TokenType`, the range checks (`is_literal`, `is_bracket`, and so on), and `to_string`. |
| `codepoint_stream.hpp` | `CodePointStream`, which walks the text one code point at a time and tracks the location. |
| `lexer.hpp` | `lexer(stream, action)`, which returns the next token. |

## Using it

```cpp
// the stream keeps a pointer, so src must outlive it
sc::src::NormalisedSource src { text, length };       
sc::lex::CodePointStream stream { src };
sc::lex::actions::TypeAndLocationAction action {};

for (auto t = lexer(stream, action); 
     t.type != TokenType::EndOfFile; 
     t = lexer(stream, action))
    ...
```

The lexer does not decide what a token looks like. The caller does, by passing an *action*, which is a struct with two
parts:
- `Output`, the type the caller wants for a token. The lexer returns `Action::Output`.
- `process`, a template on the token type, which builds an `Output` from the token's range. It returns a
  `std::optional<Output>`.

```cpp
struct MyAction {
    struct Output { 
      TokenType type; 
      src::SourceCodeRange range; 
    };

    template <TokenType T> 
    std::optional<Output> process(src::SourceCodeRange range) {
        if constexpr (sc::lex::is_whitespace(T))
            return std::nullopt;            // drop this token
        else
            return Output { T, range };     // keep this token
    }
};
```

For each token it finds, the lexer calls `action.process<TokenType::X>(range)`, with the type as a template argument.
Then:
- If `process` returns a value, the lexer returns it.
- If `process` returns `std::nullopt`, the token is dropped, and the lexer reads the next one, until some call returns a
  value. The parser uses this to drop spaces, tabs, new lines, and comments.

Because the token type is a template argument, `if constexpr` chooses what to do for each type at compile time, so the
action has no run-time cost for tokens it drops.

You may add any additionally logic that is needed.

`process` must return a value for `EndOfFile`, otherwise the lexer would never stop.

Two default actions are provided, `TypeAndLocationAction` (an `Output` of the type and range) and `TypeOnlyAction` (just the type).
The parser has its own in `sc_parser/src/tokenise.cpp`.

All lexing information can be viewed with the standalone utility.
```
build/langutils/sc_lexer/standalone/sc_lexer_standalone '"é" a 1 1ss'
```

At the end of the text the stream returns a null code point (0), even when the text does not have one. The lexer then
returns `EndOfFile`, again and again if asked.

## Tokens

`TokenType` is a plain enum, ordered in groups: identifiers, literals, keywords, brackets, punctuation, operators,
whitespace, comments, and errors. 
Each group is between a `START_OF_X` and an `END_OF_X` entry, and the `is_x` functions are range checks on them. 
Do not change the delimiters just put new tokens between them.

- Brackets are grouped by what closes them, so `get_closing_bracket` and `get_opening_brackets` are range checks too.
  `BeginClosedFunction` (`#{`) closes with `}`.
- `Minus`, `Add`, `Multiply`, `LessThan`, `GreaterThan`, `Pipe`, and `ReadWriteVar` are "ambiguous punctuation". The
  lexer can't tell if they are operators or part of the grammar, so the parser decides.
- The error tokens (`ErUnexpected`, `ErStringUnclosed`, and so on) have their own range.

## Locations and columns

A token's range is a `SourceCodeRange`. How locations, columns, and snippets work is described in the
[sc_source README](../sc_source/README.md#locations).

## How to add

**A new token.**

Doing this can break existing code, so should only be done if it is making invalid code valid, or the parser can undo this (see how while is implemented in the grammar).

1. Add it to `TokenType` between the delimiters of its group.
2. Add a case to `to_string` in `src/tokens.cpp`. `default` returns `"Unknown token"`, so a missing case compiles.
3. Lex it in `lexer.hpp`. Call `action.template process<TokenType::X>(range)`, and wrap it in `return_orelse_next_token`
   so an action can drop it.
4. Tell the parser. See [sc_parser](../sc_parser/README.md).
5. Add a test in `test/tests.cpp`.

**A new error.** 
*This is only for lexing errors.*

The same, with the entry in the errors group. 
The parser reports lexer errors itself, in `sc_parser/src/tokenise.cpp`, so add the message in `sc_diagnostic`. 
See [sc_diagnostic](../sc_diagnostic/README.md).

**A new character class.** Add it to `sc_source/codepoint.hpp`.

## Known problems

- A hexadecimal literal ignores the digits before the `x`, so `89702347890234589xAA` is the same as `0xAA`.

## Tests

`test/tests.cpp` is a Boost test. It also tests `CodePointIterator` and `LineIter` from sc_source. Run it with:

```
ctest --test-dir build -R langutils.sc_lexer_test_run
```
