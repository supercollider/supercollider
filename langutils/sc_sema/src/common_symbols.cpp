#include "sc_sema/common_symbols.hpp"
#include "sc_sema/symbol_table.hpp"

namespace sc::ir {

CommonSymbols::CommonSymbols(SymbolTable& table):
    value(table.get_selector("value")),
    new_(table.get_selector("new")),
    at(table.get_selector("at")),
    put(table.get_selector("put")),
    perform(table.get_selector("perform")),
    slot(table.get_identifier("slot")),
    float_(table.get_identifier("float")),
    int_(table.get_identifier("int")) {}
}
