#include "sc_sema/compiler.hpp"
#include "sc_sema/symbol_table.hpp"

namespace sc::ir {

CommonClasses symbol_table_and_common_classes(SymbolTable& sym_table, SymSCClassName root_object) {
    const auto abs_obj_sym = sym_table.get_class_name("AbstractObject");
    const auto obj_sym = sym_table.get_class_name("Object");
    const auto class_sym = sym_table.get_class_name("Class");
    const auto string_sym = sym_table.get_class_name("String");
    const auto true_sym = sym_table.get_class_name("True");
    const auto false_sym = sym_table.get_class_name("False");
    const auto nil_sym = sym_table.get_class_name("Nil");
    const auto float_sym = sym_table.get_class_name("Float");
    const auto int_sym = sym_table.get_class_name("Integer");
    const auto char_sym = sym_table.get_class_name("Char");
    const auto sym_sym = sym_table.get_class_name("Symbol");
    const auto array_sym = sym_table.get_class_name("Array");
    const auto event_sym = sym_table.get_class_name("Event");

    return CommonClasses {
        //
        root_object,
        obj_sym, //
        class_sym, //
        string_sym, //
        true_sym, //
        false_sym, //
        nil_sym, //
        float_sym, //
        int_sym, //
        char_sym, //
        sym_sym, //
        array_sym, //
        event_sym //
    };
}
}