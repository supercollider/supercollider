#include "sc_sema/const_expr.hpp"
#include "sc_sema/symbol_table.hpp"
#include "sc_sema/symbols.hpp"
#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstddef>
#include <iomanip>
#include <optional>
#include <sstream>
#include <string_view>

namespace sc::ir {

namespace {
// Writes a quoted json string, escaping so the output stays valid json and on one line.
void write_json_string(std::ostream& o, std::string_view s) {
    o << '"';
    for (const char c : s) {
        switch (c) {
        case '"':
            o << "\\\"";
            break;
        case '\\':
            o << "\\\\";
            break;
        case '\n':
            o << "\\n";
            break;
        case '\r':
            o << "\\r";
            break;
        case '\t':
            o << "\\t";
            break;
        case '\f':
            o << "\\f";
            break;
        case '\b':
            o << "\\b";
            break;
        default:
            if (static_cast<unsigned char>(c) < 0x20) {
                const auto flags = o.flags();
                o << "\\u" << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(c);
                o.flags(flags);
                o << std::setfill(' ');
            } else {
                o << c;
            }
        }
    }
    o << '"';
}

// The escape for c inside a SuperCollider string, symbol or char literal, if it needs one.
std::optional<char> sc_escape(char c) {
    switch (c) {
    case '\n':
        return 'n';
    case '\r':
        return 'r';
    case '\t':
        return 't';
    case '\f':
        return 'f';
    case '\v':
        return 'v';
    case '\0':
        return '0';
    case '\\':
        return '\\';
    default:
        return std::nullopt;
    }
}

// Writes s between quotes, escaping the quote, so it stays on one line.
void write_sc_quoted(std::ostream& o, std::string_view s, char quote) {
    o << quote;
    for (const char c : s) {
        if (const auto e = sc_escape(c))
            o << '\\' << *e;
        else if (c == quote)
            o << '\\' << c;
        else
            o << c;
    }
    o << quote;
}

// Whether a symbol can be written as `\name`, or used as an event key, `name: 1`.
bool is_identifier(std::string_view s) {
    if (s.empty() || !(std::isalpha(static_cast<unsigned char>(s.front())) || s.front() == '_'))
        return false;
    return std::all_of(s.begin(), s.end(),
                       [](char c) { return std::isalnum(static_cast<unsigned char>(c)) || c == '_'; });
}

// The class a typed array is an instance of, and how to write it in json.
struct TypedArrayName {
    std::string_view class_name;
    std::string_view json;
};
TypedArrayName typed_array_name(const ConstExprDoubleArray&) { return { "DoubleArray", "doublearray" }; }
TypedArrayName typed_array_name(const ConstExprFloatArray&) { return { "FloatArray", "floatarray" }; }
TypedArrayName typed_array_name(const ConstExprInt32Array&) { return { "Int32Array", "int32array" }; }
TypedArrayName typed_array_name(const ConstExprInt16Array&) { return { "Int16Array", "int16array" }; }
TypedArrayName typed_array_name(const ConstExprInt8Array&) { return { "Int8Array", "int8array" }; }
TypedArrayName typed_array_name(const ConstExprSymbolArray&) { return { "SymbolArray", "symbolarray" }; }

// An element of a typed array as the value sclang reads from it, e.g., an Int8Array's elements are Integers.
ConstExpression element(double d) { return d; }
ConstExpression element(float f) { return double { f }; }
ConstExpression element(std::int32_t i) { return i; }
ConstExpression element(std::int16_t i) { return std::int32_t { i }; }
ConstExpression element(std::int8_t i) { return std::int32_t { i }; }
ConstExpression element(SymSCSymbol s) { return s; }

// Typed arrays are written like a collection of their elements, e.g., `FloatArray[1.0, 2.5]`.
template <typename Vec> void typed_array_to_json(std::ostream& o, const Vec& vec, const SymbolTable& t) {
    o << "{\"" << typed_array_name(vec).json << "\": [";
    for (size_t i { 0 }; i < vec.size(); ++i) {
        element(vec[i]).to_json(o, t);
        if (i + 1 != vec.size())
            o << ", ";
    }
    o << "]}";
}

template <typename Vec> void typed_array_to_literal(std::ostream& o, const Vec& vec, const SymbolTable& t) {
    o << typed_array_name(vec).class_name << '[';
    for (size_t i { 0 }; i < vec.size(); ++i) {
        element(vec[i]).to_literal(o, t);
        if (i + 1 != vec.size())
            o << ", ";
    }
    o << ']';
}
}

void ConstExpression::to_json(std::ostream& o, const SymbolTable& t) const {
    visit(util::overload {
        [&](std::int32_t i) { o << "{\"int\": " << i << "}"; },
        [&](double i) {
            static constexpr auto p { 8 };
            // Json has no infinity or nan, so they are strings.
            if (std::isnan(i))
                o << "{\"float\": \"nan\"}";
            else if (std::isinf(i))
                o << "{\"float\": \"" << (i < 0 ? "-inf" : "inf") << "\"}";
            else
                o << "{\"float\": " << std::setprecision(p) << i << "}";
        },
        [&](char i) {
            o << "{\"char\": ";
            write_json_string(o, std::string_view { &i, 1 });
            o << "}";
        },
        [&](bool i) { o << "{\"bool\": " << (i ? "true" : "false") << "}"; }, //
        // Nil has no value, so the type flag holds an empty object.
        // This is done because when inserting nil into an event in sclang, it deletes the entry, so nil isn't a real
        // value, unlike json's null.
        [&](std::monostate) { o << "{\"nil\": {}}"; },
        [&](sc::ir::SymSCSymbol i) {
            o << "{\"symbol\": ";
            write_json_string(o, t(i));
            o << "}";
        },
        [&](sc::ir::SymSCClassName i) {
            o << "{\"classname\": ";
            write_json_string(o, t(i));
            o << "}";
        },
        [&](sc::ir::SymSCString i) {
            o << "{\"string\": ";
            write_json_string(o, t(i));
            o << "}";
        },
        [&](const sc::ir::ConstExprEvent& vec) {
            o << "{\"event\": [";
            for (size_t i { 0 }; i < vec.size(); ++i) {
                const auto [key, value] = vec[i];

                o << "{\"key\": ";
                key.to_json(o, t);
                o << ", ";
                o << "\"value\": ";
                value.to_json(o, t);
                o << "}";

                if (i + 1 != vec.size())
                    o << ", ";
            }
            o << "]}";
        },
        [&](const sc::ir::ConstExprArray& vec) {
            o << "{\"array\": [";
            for (size_t i { 0 }; i < vec.size(); ++i) {
                const auto value = vec[i];

                value.to_json(o, t);

                if (i + 1 != vec.size())
                    o << ", ";
            }
            o << "]}";
        },
        [&](const sc::ir::ConstExprDoubleArray& vec) { typed_array_to_json(o, vec, t); },
        [&](const sc::ir::ConstExprFloatArray& vec) { typed_array_to_json(o, vec, t); },
        [&](const sc::ir::ConstExprInt32Array& vec) { typed_array_to_json(o, vec, t); },
        [&](const sc::ir::ConstExprInt16Array& vec) { typed_array_to_json(o, vec, t); },
        [&](const sc::ir::ConstExprInt8Array& vec) { typed_array_to_json(o, vec, t); },
        [&](const sc::ir::ConstExprSymbolArray& vec) { typed_array_to_json(o, vec, t); } //
    });
}

void ConstExpression::to_literal(std::ostream& o, const SymbolTable& t) const {
    visit(util::overload {
        [&](std::int32_t i) { o << i; },
        [&](double d) {
            if (std::isinf(d)) {
                o << (d < 0 ? "-inf" : "inf");
                return;
            }
            if (std::isnan(d)) {
                o << "nan";
                return;
            }
            static constexpr auto p { 8 };
            std::ostringstream s;
            s << std::setprecision(p) << d;
            const auto text = std::move(s).str();
            // Always looks like a float, `1.0` rather than `1`.
            o << text << (text.find_first_of(".e") == std::string::npos ? ".0" : "");
        },
        [&](char c) {
            o << '$';
            if (const auto e = sc_escape(c))
                o << '\\' << *e;
            else
                o << c;
        },
        [&](bool b) { o << (b ? "true" : "false"); }, [&](std::monostate) { o << "nil"; },
        [&](sc::ir::SymSCSymbol i) {
            const auto s = t(i);
            if (is_identifier(s))
                o << '\\' << s;
            else
                write_sc_quoted(o, s, '\'');
        },
        [&](sc::ir::SymSCClassName i) { o << t(i); }, [&](sc::ir::SymSCString i) { write_sc_quoted(o, t(i), '"'); },
        [&](const sc::ir::ConstExprEvent& vec) {
            o << '(';
            for (size_t i { 0 }; i < vec.size(); ++i) {
                const auto& [key, value] = vec[i];
                const auto* symbol = key.get_if<SymSCSymbol>();
                if (symbol && is_identifier(t(*symbol)))
                    o << t(*symbol);
                else
                    key.to_literal(o, t);
                o << ": ";
                value.to_literal(o, t);
                if (i + 1 != vec.size())
                    o << ", ";
            }
            o << ')';
        },
        [&](const sc::ir::ConstExprArray& vec) {
            o << '[';
            for (size_t i { 0 }; i < vec.size(); ++i) {
                vec[i].to_literal(o, t);
                if (i + 1 != vec.size())
                    o << ", ";
            }
            o << ']';
        },
        [&](const sc::ir::ConstExprDoubleArray& vec) { typed_array_to_literal(o, vec, t); },
        [&](const sc::ir::ConstExprFloatArray& vec) { typed_array_to_literal(o, vec, t); },
        [&](const sc::ir::ConstExprInt32Array& vec) { typed_array_to_literal(o, vec, t); },
        [&](const sc::ir::ConstExprInt16Array& vec) { typed_array_to_literal(o, vec, t); },
        [&](const sc::ir::ConstExprInt8Array& vec) { typed_array_to_literal(o, vec, t); },
        [&](const sc::ir::ConstExprSymbolArray& vec) { typed_array_to_literal(o, vec, t); } //
    });
}
}
