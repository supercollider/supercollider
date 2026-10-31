#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace sc::compiler {

// What the VM defines itself, as strings, as the library's symbols don't exist yet.

/// @brief A class whose layout the VM relies on, e.g., Array or Interpreter.
struct IntrinsicClass {
    std::string name;
    std::optional<std::string> superclass; ///< Its superclass, none if it must have none.
    /// e.g., `slot` or `char`, none when it isn't indexed.
    std::optional<std::string> slot_type;
    /// Its own, not those it inherits.
    std::vector<std::string> instance_variables;
};

/// @brief A primitive's arguments, see ir::add_primitive_signature().
struct Primitive {
    std::string name;
    std::uint8_t num_arguments; ///< The number of arguments, counting the receiver.
    bool variadic_arguments;
    bool keyword_arguments;
};

/// @brief Everything the VM defines itself that the compiler needs to know.
struct VMDefinitions {
    std::vector<IntrinsicClass> classes {};
    std::vector<Primitive> primitives {};
};
}
