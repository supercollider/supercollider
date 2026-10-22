#include "sc_sema/common_symbols.hpp"
#include "sc_sema/symbol_table.hpp"

namespace sc::ir {

CommonSymbols::CommonSymbols(SymbolTable& table):
    value(table.get_selector("value")),
    new_(table.get_selector("new")),
    at(table.get_selector("at")),
    put(table.get_selector("put")),
    copy_series(table.get_selector("copySeries")),
    put_series(table.get_selector("putSeries")),
    series(table.get_selector("series")),
    add(table.get_selector("add")),
    perform(table.get_selector("perform")),
    simple_number(table.get_class_name("SimpleNumber")),
    arrayed_collection(table.get_class_name("ArrayedCollection")),
    dictionary(table.get_class_name("Dictionary")),
    object(table.get_class_name("Object")),
    this_(table.get_identifier("this")),
    super_(table.get_identifier("super")),
    this_process(table.get_identifier("thisProcess")),
    this_thread(table.get_identifier("thisThread")),
    this_method(table.get_identifier("thisMethod")),
    this_function_def(table.get_identifier("thisFunctionDef")),
    this_function(table.get_identifier("thisFunction")),
    slot(table.get_identifier("slot")),
    double_(table.get_identifier("double")),
    float_(table.get_identifier("float")),
    int32(table.get_identifier("int32")),
    int16(table.get_identifier("int16")),
    int8(table.get_identifier("int8")),
    char_(table.get_identifier("char")),
    symbol(table.get_identifier("symbol")) {}
}
