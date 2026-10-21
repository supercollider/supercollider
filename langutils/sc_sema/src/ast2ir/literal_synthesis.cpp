#include "literal_synthesis.hpp"
#include <charconv>
#include <cmath>
#include <cstdlib>
#include <iterator>
#include <sstream>

#include "sc_parser/ast.hpp"

namespace sc::ir {

namespace priv {
// Copied from the original source
inline double sc_strtof(const char* str, int n, int base) {
    double z = 0.;
    int decptpos = 0;
    for (int i = 0; i < n; ++i) {
        int c = *str++;
        if (!c)
            break;
        if (c >= '0' && c <= '0' + std::min(10, base) - 1)
            z = z * base + c - '0';
        else if (c >= 'a' && c <= 'a' + std::min(36, base) - 11)
            z = z * base + c - 'a' + 10;
        else if (c >= 'A' && c <= 'A' + std::min(36, base) - 11)
            z = z * base + c - 'A' + 10;
        else if (c == '.')
            decptpos = i;
    }
    // calculation previously included decimal point in count of columns (was n-decptpos); there are 1 less than n
    // characters which are columns in the number contribution
    z = z / std::pow(static_cast<double>(base), n - 1 - decptpos);
    return z;
}

inline int sc_strtoi(const char* str, int n, int base) {
    int z = 0;
    for (int i = 0; i < n; ++i) {
        int c = *str++;
        if (!c)
            break;
        if (c >= '0' && c <= '0' + std::min(10, base) - 1)
            z = z * base + c - '0';
        else if (c >= 'a' && c <= 'a' + std::min(36, base) - 11)
            z = z * base + c - 'a' + 10;
        else if (c >= 'A' && c <= 'A' + std::min(36, base) - 11)
            z = z * base + c - 'A' + 10;
    }
    return z;
}

// from cpp ref
[[noreturn]] inline void unreachable() {
    // Uses compiler specific extensions if possible.
    // Even if no extension is used, undefined behavior is still raised by
    // an empty function body and the noreturn attribute.
#if defined(_MSC_VER) && !defined(__clang__) // MSVC
    __assume(false);
#else // GCC, Clang
    __builtin_unreachable();
#endif
}
}

template <typename T> std::optional<T> from(const char* begin, const char* end) {
    T t;
    const auto [ptr, er] = std::from_chars(begin, end, t);
    if (ptr != end)
        return {};
    if (er != std::errc())
        return {};
    return { t };
}
template <typename T> std::optional<T> from(std::string_view v) { return from<T>(v.begin(), v.end()); }

int synthesize_literal(const ast::IntNode& node, std::string_view str) {
    static constexpr size_t len { 128 };
    char buf[len] {};
    assert(str.size() < len);
    std::copy(str.begin(), str.end(), buf);
    buf[str.size()] = 0;

    switch (node.kind) {
    case ast::IntNode::Kind::Normal: {
        auto i = from<int>(str);
        assert(i);
        return *i;
    }
    case ast::IntNode::Kind::Hexadecimal: {
        auto it = str.begin();
        const auto end = str.end();
        while (*it != 'x' && *it != 'X') {
            assert(it < str.end()); // lexer should ensure this.
            ++it;
        }
        ++it; // skip x
        int val = 0;
        while (it < end) {
            if (*it >= '0' && *it <= '9')
                val = val * 16 + *it - '0';
            else if (*it >= 'a' && *it <= 'z')
                val = val * 16 + *it - 'a' + 10;
            else if (*it >= 'A' && *it <= 'Z')
                val = val * 16 + *it - 'A' + 10;
            it++;
        }
        return val;
    }

    case ast::IntNode::Kind::Radix: {
        auto it = str.begin();
        while (*it != 'r') {
            assert(it < str.end()); // lexer should ensure this.
            ++it;
        }
        auto radix = from<int>(str.begin(), it);
        assert(radix);
        ++it; // drop r
        assert(it != str.end());
        return priv::sc_strtoi(it, std::distance(it, str.end()), *radix);
    }
    default:
        priv::unreachable();
        return 0;
    }
}
char synthesize_literal(const ast::ASCIINode&, std::string_view str) {
    assert(str.front() == '$');
    assert(str.size() == 2 || str.size() == 3);
    if (str.size() == 2) {
        assert(str[1] != 0); // lexer ensures this.
        return str[1];
    } else {
        assert(str[1] == '\\');
        switch (str[2]) {
        case 'n':
            return '\n';
        case 'r':
            return '\r';
        case 't':
            return '\t';
        case 'f':
            return '\f';
        case 'v':
            return '\v';
        case '0':
            return '\0';
        default:
            return str[2];
        }
    }
}
double synthesize_literal(const ast::FloatNode& node, std::string_view str) {
    static constexpr size_t len { 128 };
    char buf[len] {};
    // This would be a floating point number with 128 characters.
    assert(str.size() < len);
    std::copy(str.begin(), str.end(), buf);
    buf[str.size()] = 0;


    switch (node.kind) {
    case ast::FloatNode::Kind::Inf:
        return std::numeric_limits<double>::infinity();

    case ast::FloatNode::Kind::Exponent:
        [[fallthrough]];
    case ast::FloatNode::Kind::Normal: {
        auto r = from<double>(str.begin(), str.end());
        assert(r);
        return *r;
    }

    case ast::FloatNode::Kind::Radix: {
        auto it = str.begin();
        while (*it != 'r') {
            assert(it < str.end()); // lexer should ensure this.
            ++it;
        }
        auto radix = from<int>(str.begin(), it);
        assert(radix);
        ++it; // drop r
        assert(it != str.end());
        return priv::sc_strtof(it, std::distance(it, str.end()), *radix);
    }
    default:
        priv::unreachable();
        return 0;
    }
}
std::string synthesize_literal(const ast::SymbolQuote&, std::string_view str) {
    // See github issue: #7783
    // '\n' === 'n'
    // '\n' !== "\n".asSymbol

    assert(str.front() == '\'');
    assert(str.back() == '\'');
    str.remove_prefix(1);
    str.remove_suffix(1);

    std::stringstream ss;

    bool escaped = false;
    for (auto it = str.begin(); it < str.end(); ++it) {
        if (*it == '\\' && !escaped) {
            escaped = true;
            continue;
        }
        ss << *it;
        escaped = false;
    }
    return std::move(ss).str();
}
std::string_view synthesize_literal(const ast::SymbolSlash&, std::string_view str) {
    assert(str.front() == '\\');
    str.remove_prefix(1);
    return str;
}
std::string_view synthesize_literal(const ast::KeyBinOp&, std::string_view str) {
    assert(str.back() == ':');
    str.remove_suffix(1);
    return str;
}
std::string synthesize_literal(ast::StringLitIndex i, const ast::ASTGraph& ast_graph) {
    std::stringstream out;
    for (auto it = ast_graph.children(i).iter(); it; it += 1) {
        auto line = ast_graph.string_view(*it);

        assert(line.size() >= 2);
        assert(line.front() == '"');
        assert(line.back() == '"');
        // removes quotes
        line.remove_suffix(1);
        line.remove_prefix(1);

        bool escaped = false;
        for (auto ch = line.begin(); ch < line.end(); ++ch) {
            if (*ch == '\\' && !escaped) {
                escaped = true;
                continue;
            }
            if (escaped) {
                switch (*ch) {
                case 'n':
                    out << '\n';
                    break;
                case 'r':
                    out << '\r';
                    break;
                case 't':
                    out << '\t';
                    break;
                case 'f':
                    out << '\f';
                    break;
                case 'v':
                    out << '\v';
                    break;
                default:
                    out << *ch;
                    break;
                }
                escaped = false;
            } else {
                out << *ch;
            }
        }
    }
    return std::move(out).str();
}
bool synthesize_literal(const ast::BooleanNode&, std::string_view str) { return str == "true"; }
ast::ReadWriteAccessor synthesize_literal(const ast::MaybeRWAccessorIndex i, const ast::ASTGraph& g) {
    if (const auto m = g.as_a<ast::RWAccessorIndex>(i))
        return g.payload(*m).accessor;
    else
        return ast::ReadWriteAccessor::Private;
}
}