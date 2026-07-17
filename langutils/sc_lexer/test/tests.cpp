#define BOOST_TEST_MODULE sc_lexer_tests
#include "tokens.hpp"
#include <boost/test/included/unit_test.hpp>
#include <cstddef>
#include <source_utils.hpp>
#include <lexer.hpp>
#include <normalise_source.hpp>

using namespace sc::lex;
// MSVC needs this.

BOOST_TEST_DONT_PRINT_LOG_VALUE(sc::lex::TokenType);

BOOST_AUTO_TEST_CASE(codepointer_iterator_forward) {
    using CPI = utils::CodePointIterator;
    const char* const source = "Here is some text.\n∃\t∑\n.";
    auto m_it = CPI::make(source, source + strlen(source) + 1);
    BOOST_TEST(m_it.has_value());
    auto it = std::move(*m_it);

    const std::array<sc::lex::CodePoint, 25> expected {
        'H', 'e', 'r', 'e', ' ', 'i',  's',    ' ',  's',    'o',  'm', 'e', ' ',
        't', 'e', 'x', 't', '.', '\n', 0x2203, '\t', 0x2211, '\n', '.', 0,
    };

    std::size_t i = 0;
    for (auto r = it.forwards(); r; r = it.forwards()) {
        BOOST_TEST(expected[i] == *r);
        ++i;
    }

    BOOST_TEST(i == expected.size());
}

BOOST_AUTO_TEST_CASE(codepointer_iterator_backward) {
    using CPI = utils::CodePointIterator;
    const char* const source = "Here is some text.\n∃\t∑\n.";
    auto m_it = CPI::make(source, source + strlen(source) + 1, source + strlen(source) + 1);
    BOOST_TEST(m_it.has_value());
    auto it = std::move(*m_it);

    const std::array<sc::lex::CodePoint, 25> expected {
        'H', 'e', 'r', 'e', ' ', 'i',  's',    ' ',  's',    'o',  'm', 'e', ' ',
        't', 'e', 'x', 't', '.', '\n', 0x2203, '\t', 0x2211, '\n', '.', 0,
    };

    std::size_t i = 0;
    for (auto r = it.backwards(); r; r = it.backwards()) {
        BOOST_TEST(expected[24 - i] == *r);
        ++i;
    }

    BOOST_TEST(i == expected.size());
}


BOOST_AUTO_TEST_CASE(line_iter_forwards) {
    const char* const source = "A\n"
                               "b\n"
                               "c\n";
    utils::LineIter iter = [&]() {
        auto r = utils::LineIter::make(source, source + strlen(source) + 1);
        BOOST_TEST(r.has_value());
        return *r;
    }();

    {
        std::array<const char*, 4> expected { { "A\n", "b\n", "c\n", "" } };

        for (const char* e : expected) {
            const auto r = iter.forwards();
            BOOST_TEST(r.has_value());
            const auto [ptr, sz, line, end_in_new_line] = *r;
            for (size_t i { 0 }; i < sz; ++i) {
                BOOST_TEST(ptr[i] == e[i]);
            }
        }
        BOOST_TEST(!iter.forwards().has_value());
    }
    {
        std::array<const char*, 4> expected { { "", "c\n", "b\n", "A\n" } };

        for (const char* e : expected) {
            const auto r = iter.backwards();
            BOOST_TEST(r.has_value());
            const auto [ptr, sz, line, end_in_new_line] = *r;
            for (size_t i { 0 }; i < sz; ++i) {
                BOOST_TEST(ptr[i] == e[i]);
            }
        }
        BOOST_TEST(!iter.backwards().has_value());
    }
}

BOOST_AUTO_TEST_CASE(line_iter_backwards) {
    const char* const source = "A\n"
                               "b\n"
                               "c\n";
    utils::LineIter iter = [&]() {
        const auto len = strlen(source);
        auto r = utils::LineIter::make(source, source + len + 1, { len + 1, 2, 2 });
        BOOST_TEST(r.has_value());
        return *r;
    }();


    {
        std::array<const char*, 4> expected { { "", "c\n", "b\n", "A\n" } };

        for (const char* e : expected) {
            const auto r = iter.backwards();
            BOOST_TEST(r.has_value());
            const auto [ptr, sz, line, end_in_new_line] = *r;
            for (size_t i { 0 }; i < sz; ++i) {
                BOOST_TEST(ptr[i] == e[i]);
            }
        }
        BOOST_TEST(!iter.backwards().has_value());
    }

    {
        std::array<const char*, 4> expected { { "A\n", "b\n", "c\n", "" } };

        for (const char* e : expected) {
            const auto r = iter.forwards();
            BOOST_TEST(r.has_value());
            const auto [ptr, sz, line, end_in_new_line] = *r;
            for (size_t i { 0 }; i < sz; ++i) {
                BOOST_TEST(ptr[i] == e[i]);
            }
        }
        BOOST_TEST(!iter.forwards().has_value());
    }
}


static constexpr std::array default_gobble {
    sc::lex::TokenType::Space,   sc::lex::TokenType::NewLine,          sc::lex::TokenType::Tab,
    sc::lex::TokenType::Comment, sc::lex::TokenType::MultilineComment,
};

template <size_t N, size_t M>
void match(const char* text, const std::array<sc::lex::TokenType, N>& to_find,
           const std::array<sc::lex::TokenType, M>& to_gobble) {
    CodePointStream stream { NormalisedSource(text, strlen(text)), {} };

    sc::lex::actions::TypeAndLocationAction action {};

    for (const sc::lex::TokenType t : to_find) {
        auto o = lexer(stream, action);

        while (std::find(to_gobble.begin(), to_gobble.end(), o.type) != to_gobble.end()) {
            o = lexer(stream, action);
        }

        BOOST_TEST(o.type == t);
    }

    auto o = lexer(stream, action);
    while (std::find(to_gobble.begin(), to_gobble.end(), o.type) != to_gobble.end()
           && o.type != sc::lex::TokenType::EndOfFile) {
        o = lexer(stream, action);
    }

    BOOST_TEST(o.type == sc::lex::TokenType::EndOfFile);
}

BOOST_AUTO_TEST_CASE(basic) {
    const char* text = "   some 0.312 \\hello \n 'text';  \n"
                       "\n"
                       "\n"
                       "-0.2 \t  _Pri Foo _T:\\6( ";

    // NO gobble
    match(text,
          std::array {
              sc::lex::TokenType::Space,
              sc::lex::TokenType::Name,
              sc::lex::TokenType::Space,
              sc::lex::TokenType::Float,
              sc::lex::TokenType::Space,
              sc::lex::TokenType::SymbolSlash,
              sc::lex::TokenType::Space,
              sc::lex::TokenType::NewLine,
              sc::lex::TokenType::Space,
              sc::lex::TokenType::SymbolQuote,
              sc::lex::TokenType::SemiColon,
              sc::lex::TokenType::Space,
              sc::lex::TokenType::NewLine,
              sc::lex::TokenType::Minus,
              sc::lex::TokenType::Float,
              sc::lex::TokenType::Space,
              sc::lex::TokenType::Tab,
              sc::lex::TokenType::Space,
              sc::lex::TokenType::PrimitiveName,
              sc::lex::TokenType::Space,
              sc::lex::TokenType::ClassName,
              sc::lex::TokenType::Space,
              sc::lex::TokenType::KeywordBinaryOperator,
              sc::lex::TokenType::SymbolSlash,
              sc::lex::TokenType::OpenParen,
              sc::lex::TokenType::Space,
              sc::lex::TokenType::EndOfFile,
          },
          std::array<sc::lex::TokenType, 0> {});

    match("    *new ", std::array { sc::lex::TokenType::Multiply, sc::lex::TokenType::Name }, default_gobble);

    match("    const nl = \"\\n\"; \n\t*new ",
          std::array {
              sc::lex::TokenType::Const,
              sc::lex::TokenType::Name,
              sc::lex::TokenType::EqualsSign,
              sc::lex::TokenType::StringLine,
              sc::lex::TokenType::SemiColon,
              sc::lex::TokenType::Multiply,
              sc::lex::TokenType::Name,
          },
          default_gobble);
}

BOOST_AUTO_TEST_CASE(fn) {
    const char* text = "       \t\t\t\t\t\t        var f = {|a,b,c|\na+b + c}\n\n\t;   ";

    match(text,
          std::array {
              sc::lex::TokenType::Var,
              sc::lex::TokenType::Name,
              sc::lex::TokenType::EqualsSign,
              sc::lex::TokenType::OpenCurly,
              sc::lex::TokenType::Pipe,
              sc::lex::TokenType::Name,
              sc::lex::TokenType::Comma,
              sc::lex::TokenType::Name,
              sc::lex::TokenType::Comma,
              sc::lex::TokenType::Name,
              sc::lex::TokenType::Pipe,
              sc::lex::TokenType::Name,
              sc::lex::TokenType::Add,
              sc::lex::TokenType::Name,
              sc::lex::TokenType::Add,
              sc::lex::TokenType::Name,
              sc::lex::TokenType::CloseCurly,
              sc::lex::TokenType::SemiColon,
          },
          default_gobble);
}

BOOST_AUTO_TEST_CASE(strings) {
    match(R"%(   "(\""   )%", std::array { sc::lex::TokenType::StringLine }, default_gobble);
    match(R"%( "(\"" )%", std::array { sc::lex::TokenType::StringLine }, default_gobble);
    match(R"%( "\")" abs )%", std::array { sc::lex::TokenType::StringLine, sc::lex::TokenType::Name }, default_gobble);
    match(R"%( "◎" bang )%", std::array { sc::lex::TokenType::StringLine, sc::lex::TokenType::Name }, default_gobble);
    match(R"%( 
			"The function % should behave the same for a Pasc::lex::TokenTypeernProxy and its source:\n%\n"
    )%",
          std::array { sc::lex::TokenType::StringLine }, default_gobble);
}

BOOST_AUTO_TEST_CASE(symbol) {
    match("\\)", std::array { sc::lex::TokenType::SymbolSlash, sc::lex::TokenType::CloseParen }, default_gobble);
}

BOOST_AUTO_TEST_CASE(ascii) {
    match("$a", std::array { sc::lex::TokenType::Ascii }, default_gobble);
    match("$a)", std::array { sc::lex::TokenType::Ascii, sc::lex::TokenType::CloseParen }, default_gobble);
    match("$\\n", std::array { sc::lex::TokenType::Ascii }, default_gobble);
    match("$\\n)", std::array { sc::lex::TokenType::Ascii, sc::lex::TokenType::CloseParen }, default_gobble);
    match("$ ", std::array { sc::lex::TokenType::Ascii }, default_gobble);
    match("$    bang  ", std::array { sc::lex::TokenType::Ascii, sc::lex::TokenType::Name }, default_gobble);
}

BOOST_AUTO_TEST_CASE(larger_obj) {
    const auto txt = R"%%(
Object {
	classvar <dependantsDictionary, currentEnvironment, topEnvironment, <uniqueMethods;

	const nl = "\n";

	*new { arg maxSize = 0; _BasicNew
    )%%";
    using T = sc::lex::TokenType;
    // clang-format off
    match(txt,
          std::array {
              T::ClassName,  sc::lex::TokenType::OpenCurly, 
              T::ClassVar,
              sc::lex::TokenType::LessThan, T::Name, sc::lex::TokenType::Comma,
              T::Name, sc::lex::TokenType::Comma,
              T::Name,sc::lex::TokenType::Comma, 
              sc::lex::TokenType::LessThan, T::Name,sc::lex::TokenType::SemiColon, 
              T::Const, T::Name, sc::lex::TokenType::EqualsSign, T::StringLine, sc::lex::TokenType::SemiColon,
              sc::lex::TokenType::Multiply, T::Name, sc::lex::TokenType::OpenCurly,
              T::Arg, T::Name, sc::lex::TokenType::EqualsSign, T::Integer, sc::lex::TokenType::SemiColon,
              T::PrimitiveName
          },
          default_gobble);
    // clang-format on
}

BOOST_AUTO_TEST_CASE(recorder) {
    const auto txt = R"%%(
Recorder {

	var <server, <>numChannels;
	var >recHeaderFormat, >recSampleFormat, >recBufSize;
	var recordBuf, recordNode, synthDef;
    )%%";
    using T = sc::lex::TokenType;
    // clang-format off
    match(txt,
          std::array {
              T::ClassName,  sc::lex::TokenType::OpenCurly, 
              T::Var,
              sc::lex::TokenType::LessThan, T::Name, sc::lex::TokenType::Comma,
              T::ReadWriteVar, T::Name, sc::lex::TokenType::SemiColon,

              T::Var,
              sc::lex::TokenType::GreaterThan, T::Name, sc::lex::TokenType::Comma,
              sc::lex::TokenType::GreaterThan, T::Name, sc::lex::TokenType::Comma,
              sc::lex::TokenType::GreaterThan, T::Name, sc::lex::TokenType::SemiColon,

              T::Var,
              T::Name, sc::lex::TokenType::Comma,
              T::Name, sc::lex::TokenType::Comma,
              T::Name, sc::lex::TokenType::SemiColon,
          },
          default_gobble);
    // clang-format on
}


using TSource = std::pair<sc::lex::TokenType, const char*>;
template <size_t I> void match_str(const char* text, const std::array<TSource, I>& expected) {
    CodePointStream stream { sc::lex::NormalisedSource(text, strlen(text)), {} };
    sc::lex::actions::TypeAndLocationAction action {};

    const auto test = [&](auto o, sc::lex::TokenType t, const char* txt) {
        BOOST_TEST(o.type == t);
        const auto [ptr, sz] = stream.source_code_range_to_text(o.range);
        const auto txt_sz = strlen(txt);
        BOOST_TEST(sz == txt_sz);
        for (size_t i { 0 }; i < sz; ++i)
            BOOST_TEST(ptr[i] == txt[i]);
    };

    for (const auto& e : expected) {
        test(sc::lex::lexer(stream, action), e.first, e.second);
    }
    test(sc::lex::lexer(stream, action), sc::lex::TokenType::EndOfFile, "");
}

BOOST_AUTO_TEST_CASE(newline_normalisation) {
    match_str("meow\r\nwoof",
              std::array {
                  TSource { sc::lex::TokenType::Name, "meow" },
                  TSource { sc::lex::TokenType::NewLine, "\n" },
                  TSource { sc::lex::TokenType::Name, "woof" },
              });
    match_str("meow\nwoof",
              std::array {
                  TSource { sc::lex::TokenType::Name, "meow" },
                  TSource { sc::lex::TokenType::NewLine, "\n" },
                  TSource { sc::lex::TokenType::Name, "woof" },
              });
    match_str("meow\rwoof",
              std::array {
                  TSource { sc::lex::TokenType::Name, "meow" },
                  TSource { sc::lex::TokenType::NewLine, "\n" },
                  TSource { sc::lex::TokenType::Name, "woof" },
              });
    match_str("meow\r\n\rwoof",
              std::array {
                  TSource { sc::lex::TokenType::Name, "meow" },
                  TSource { sc::lex::TokenType::NewLine, "\n\n" },
                  TSource { sc::lex::TokenType::Name, "woof" },
              });
    match_str("meow\n\rwoof",
              std::array {
                  TSource { sc::lex::TokenType::Name, "meow" },
                  TSource { sc::lex::TokenType::NewLine, "\n\n" },
                  TSource { sc::lex::TokenType::Name, "woof" },
              });
    match_str("meow\r\n\rwoof",
              std::array {
                  TSource { sc::lex::TokenType::Name, "meow" },
                  TSource { sc::lex::TokenType::NewLine, "\n\n" },
                  TSource { sc::lex::TokenType::Name, "woof" },
              });
    match_str("meow\r\r\n\rwoof",
              std::array {
                  TSource { sc::lex::TokenType::Name, "meow" },
                  TSource { sc::lex::TokenType::NewLine, "\n\n\n" },
                  TSource { sc::lex::TokenType::Name, "woof" },
              });


    // Turn all this stuff that unicode considers a newline, into the \n character.
    match_str("meow\f\v\n\rwoof",
              std::array {
                  TSource { sc::lex::TokenType::Name, "meow" },
                  TSource { sc::lex::TokenType::NewLine, "\n\n\n\n" },
                  TSource { sc::lex::TokenType::Name, "woof" },
              });
    match_str("\"meow\f\v\n\rwoof\"",
              std::array {
                  TSource { sc::lex::TokenType::StringLine, "\"meow\n\n\n\nwoof\"" },
              });
}
