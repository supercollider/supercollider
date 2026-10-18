#include "sc_sema/const_expr.hpp"
#include "sc_sema/symbol_table.hpp"
#include "sc_sema/symbols.hpp"
#include <cstddef>
#include <iomanip>

namespace sc::ir {

void ConstExpression::to_json(std::ostream& o, const SymbolTable& t) const {
    visit(util::overload {
        [&](std::int32_t i) { o << "{\"int\": " << i << "}"; },
        [&](double i) {
            static constexpr auto p { 8 };
            o << "{\"float\": " << std::setprecision(p) << i << "}";
        },
        [&](char i) { o << "{\"char\": " << i << "}"; }, [&](bool i) { o << "{\"bool\": " << i << "}"; }, //
        [&](std::monostate) { o << "{\"nil\": nil}"; },
        [&](sc::ir::SymSCSymbol i) { o << "{\"symbol\": \"" << t(i) << "\"}"; },
        [&](sc::ir::SymSCClassName i) { o << "{\"classname\": \"" << t(i) << "\"}"; },
        [&](sc::ir::SymSCString i) { o << "{\"string\": \"" << t(i) << "\"}"; },
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
