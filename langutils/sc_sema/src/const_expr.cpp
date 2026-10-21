#include "sc_sema/const_expr.hpp"
#include "sc_sema/symbol_table.hpp"
#include "sc_sema/symbols.hpp"
#include <cstddef>
#include <iomanip>
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
}

void ConstExpression::to_json(std::ostream& o, const SymbolTable& t) const {
    visit(util::overload {
        [&](std::int32_t i) { o << "{\"int\": " << i << "}"; },
        [&](double i) {
            static constexpr auto p { 8 };
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
        } //
    });
}

}
