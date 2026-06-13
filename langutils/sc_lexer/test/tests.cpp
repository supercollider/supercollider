#define BOOST_TEST_MODULE sc_lexer_tests
#include <boost/test/included/unit_test.hpp>
#include <cstddef>
#include <source_utils.hpp>
#include <lexer.hpp>

using namespace sc::lex;
// MSVC needs this.
using TT = sc::lex::TokenType;

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
    TT::Space, TT::NewLine, TT::Tab, TT::Comment, TT::MultilineComment,
};

template <size_t N, size_t M>
void match(const char* text, const std::array<TT, N>& to_find, const std::array<TT, M>& to_gobble) {
    const auto text_len = strlen(text);

    CodePointStream stream { text, text_len, {} };

    sc::lex::actions::TypeAndLocationAction action {};

    for (const TT t : to_find) {
        auto o = lexer(stream, action);

        while (std::find(to_gobble.begin(), to_gobble.end(), o.type) != to_gobble.end()) {
            o = lexer(stream, action);
        }

        BOOST_TEST(o.type == t);
    }

    auto o = lexer(stream, action);
    while (std::find(to_gobble.begin(), to_gobble.end(), o.type) != to_gobble.end() && o.type != TT::EndOfFile) {
        o = lexer(stream, action);
    }

    BOOST_TEST(o.type == TT::EndOfFile);
}

BOOST_AUTO_TEST_CASE(basic) {
    const char* text = "   some 0.312 \\hello \n 'text';  \n"
                       "\n"
                       "\n"
                       "-0.2 \t  _Pri Foo _T:\\6( ";

    // NO gobble
    match(text,
          std::array {
              TT::Space,
              TT::Name,
              TT::Space,
              TT::Float,
              TT::Space,
              TT::SymbolSlash,
              TT::Space,
              TT::NewLine,
              TT::Space,
              TT::SymbolQuote,
              TT::SemiColon,
              TT::Space,
              TT::NewLine,
              TT::Minus,
              TT::Float,
              TT::Space,
              TT::Tab,
              TT::Space,
              TT::PrimitiveName,
              TT::Space,
              TT::ClassName,
              TT::Space,
              TT::KeywordBinaryOperator,
              TT::SymbolSlash,
              TT::OpenParen,
              TT::Space,
              TT::EndOfFile,
          },
          std::array<TT, 0> {});

    match("    *new ", std::array { TT::Multiply, TT::Name }, default_gobble);

    match("    const nl = \"\\n\"; \n\t*new ",
          std::array {
              TT::Const,
              TT::Name,
              TT::EqualsSign,
              TT::StringLine,
              TT::SemiColon,
              TT::Multiply,
              TT::Name,
          },
          default_gobble);
}

BOOST_AUTO_TEST_CASE(fn) {
    const char* text = "       \t\t\t\t\t\t        var f = {|a,b,c|\na+b + c}\n\n\t;   ";

    match(text,
          std::array {
              TT::Var,
              TT::Name,
              TT::EqualsSign,
              TT::OpenCurly,
              TT::Pipe,
              TT::Name,
              TT::Comma,
              TT::Name,
              TT::Comma,
              TT::Name,
              TT::Pipe,
              TT::Name,
              TT::Add,
              TT::Name,
              TT::Add,
              TT::Name,
              TT::CloseCurly,
              TT::SemiColon,
          },
          default_gobble);
}

BOOST_AUTO_TEST_CASE(strings) {
    match(R"%(   "(\""   )%", std::array { TT::StringLine }, default_gobble);
    match(R"%( "(\"" )%", std::array { TT::StringLine }, default_gobble);
    match(R"%( "\")" abs )%", std::array { TT::StringLine, TT::Name }, default_gobble);
    match(R"%( "◎" bang )%", std::array { TT::StringLine, TT::Name }, default_gobble);
    match(R"%( 
			"The function % should behave the same for a PatternProxy and its source:\n%\n"
    )%",
          std::array { TT::StringLine }, default_gobble);
}

BOOST_AUTO_TEST_CASE(symbol) { match("\\)", std::array { TT::SymbolSlash, TT::CloseParen }, default_gobble); }

BOOST_AUTO_TEST_CASE(ascii) {
    match("$a", std::array { TT::Ascii }, default_gobble);
    match("$a)", std::array { TT::Ascii, TT::CloseParen }, default_gobble);
    match("$\\n", std::array { TT::Ascii }, default_gobble);
    match("$\\n)", std::array { TT::Ascii, TT::CloseParen }, default_gobble);
    match("$ ", std::array { TT::Ascii }, default_gobble);
    match("$    bang  ", std::array { TT::Ascii, TT::Name }, default_gobble);
}

BOOST_AUTO_TEST_CASE(larger_obj) {
    const auto txt = R"%%(
Object {
	classvar <dependantsDictionary, currentEnvironment, topEnvironment, <uniqueMethods;

	const nl = "\n";

	*new { arg maxSize = 0; _BasicNew
    )%%";
    using T = TT;
    // clang-format off
    match(txt,
          std::array {
              T::ClassName,  TT::OpenCurly, 
              T::ClassVar,
              TT::LessThan, T::Name, TT::Comma,
              T::Name, TT::Comma,
              T::Name,TT::Comma, 
              TT::LessThan, T::Name,TT::SemiColon, 
              T::Const, T::Name, TT::EqualsSign, T::StringLine, TT::SemiColon,
              TT::Multiply, T::Name, TT::OpenCurly,
              T::Arg, T::Name, TT::EqualsSign, T::Integer, TT::SemiColon,
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
    using T = TT;
    // clang-format off
    match(txt,
          std::array {
              T::ClassName,  TT::OpenCurly, 
              T::Var,
              TT::LessThan, T::Name, TT::Comma,
              T::ReadWriteVar, T::Name, TT::SemiColon,

              T::Var,
              TT::GreaterThan, T::Name, TT::Comma,
              TT::GreaterThan, T::Name, TT::Comma,
              TT::GreaterThan, T::Name, TT::SemiColon,

              T::Var,
              T::Name, TT::Comma,
              T::Name, TT::Comma,
              T::Name, TT::SemiColon,
          },
          default_gobble);
    // clang-format on
}
