#include "sc_diagnostic/sc_diagnostic.hpp"
#include "sc_diagnostic/highlight.hpp"
#include "sc_lexer/tokens.hpp"
#include "sc_source/source_utils.hpp"
#include <filesystem>
#include <sstream>


namespace sc::diag {

std::string diagnosticToString(Severity type, const char* generalDescription,
                               const std::vector<Diagnostic::Location>& locations, const std::string& extra,
                               const std::filesystem::path* relative_to = nullptr) {
    std::stringstream ss;

    ss << "\n──────────────────────────────────────────────────────────────────────────────────\n";

    switch (type) {
    case Severity::Error:
        ss << "Error: ";
        break;
    case Severity::Warning:
        ss << "Warning: ";
        break;
    }

    if (generalDescription)
        ss << generalDescription;

    ss << '\n';

    std::vector<Highlight> highlights;
    for (const auto& loc : locations) {
        const auto& file_path = loc.text_info->file_path;
        highlights.push_back({ file_path ? &*file_path : nullptr, loc.text_info->source.as_string(), loc.range,
                               loc.text_info->source_start_in_file, loc.msg });
    }
    streamHighlights(ss, highlights, true, {}, relative_to);

    if (!extra.empty()) {
        ss << "\n" << extra;
    }

    ss << "\n──────────────────────────────────────────────────────────────────────────────────\n";
    return ss.str();
}


Diagnostic::Diagnostic(const char* name, Severity severity, std::vector<Location> location, std::string extra) noexcept:
    m_locations(std::move(location)),
    extra_info(std::move(extra)),
    m_name(name),
    m_severity(severity) {}

const std::vector<Diagnostic::Location>& Diagnostic::locations() const { return m_locations; }

Severity Diagnostic::severity() const { return m_severity; }

bool Diagnostic::fatal() const { return m_severity == Severity::Error; }

bool Diagnostic::within(const TextInfoPtr& text, sc::src::SourceCodeRange range) const {
    if (m_locations.empty())
        return false;
    const auto& l = m_locations.back();
    return l.text_info == text && range.begin.absolute <= l.range.begin.absolute
        && l.range.end.absolute <= range.end.absolute;
}

[[nodiscard]] const char* Diagnostic::name() const { return m_name; }

std::string Diagnostic::to_string() const { return diagnosticToString(severity(), name(), locations(), extra_info); }

std::string Diagnostic::to_string(const std::filesystem::path& relative_to) const {
    return diagnosticToString(severity(), name(), locations(), extra_info, &relative_to);
}

////////////////////////////////////////////////////////////////////////////////

Diagnostic Diagnostic::unexpectedToken(std::shared_ptr<const sc::src::TextInfo> text_info, sc::src::SourceCodeRange loc,
                                       std::string_view expected, std::string_view found) {
    return { "Unexpected Token.", //
             Severity::Error, //
             std::vector<Location> { {
                 std::move(text_info), //
                 loc, //
                 "Expected " + std::string { expected } + ", but found " + std::string { found } + "." //
             } } };
};

namespace {
// The first count bytes of the range, e.g., the opening quote of an unclosed string, rather than the rest of the file.
sc::src::SourceCodeRange opening(sc::src::SourceCodeRange loc, std::uint32_t count) {
    auto end = loc.begin;
    end.absolute += count;
    end.column += count;
    return { loc.begin, end };
}
}

Diagnostic Diagnostic::lexer_error(std::shared_ptr<const sc::src::TextInfo> text_info, sc::lex::TokenType token,
                                   sc::src::SourceCodeRange loc) {
    using T = sc::lex::TokenType;
    const auto make = [&](const char* title, std::string msg, sc::src::SourceCodeRange range) -> Diagnostic {
        return { title, Severity::Error, std::vector<Location> { { std::move(text_info), range, std::move(msg) } } };
    };
    switch (token) {
    case T::ErUnexpected:
        return make("Unexpected Character.", "`" + std::string { text_info->read(loc) } + "` can't start a token.",
                    loc);
    case T::ErUnexpectedUnicode:
        return make("Unexpected Character.",
                    "Only ASCII characters may be used outside of strings, symbols, and comments.", loc);
    case T::ErSymbolQuoteUnclosed:
        return make("Unclosed Symbol.", "This symbol is never closed with `'`.", opening(loc, 1));
    case T::ErMultilineCommentUnclosed:
        return make("Unclosed Comment.", "This comment is never closed with `*/`.", opening(loc, 2));
    case T::ErStringUnclosed:
        return make("Unclosed String.", "This string is never closed with `\"`.", opening(loc, 1));
    case T::ErASCIIInvalidWhitespace:
        return make("Invalid Character.",
                    "The only whitespace `$` may be followed by is a space, write a tab or new line as `$\\t` or "
                    "`$\\n`.",
                    loc);
    case T::ErASCIIEOF:
        return make("Invalid Character.", "`$` must be followed by a character, but the file ends.", loc);
    case T::ErASCIINotASCII:
        return make("Invalid Character.", "`$` must be followed by an ASCII character.", loc);
    case T::ErMissingExponent:
        return make("Missing Exponent.", "The exponent has no digits.", loc);
    default:
        return make("Invalid Token.", "This isn't a valid token.", loc);
    }
}

namespace {
// The bracket that closes the opening one, e.g., `}` for `#{`.
std::string_view closing(std::string_view open) {
    switch (open.back()) {
    case '(':
        return ")";
    case '[':
        return "]";
    default:
        return "}";
    }
}

std::string quoted(std::string_view s) { return "`" + std::string { s } + "`"; }
}

Diagnostic Diagnostic::invalid_utf8(std::shared_ptr<const sc::src::TextInfo> text_info, sc::src::SourceCodeRange first,
                                    std::size_t others) {
    auto msg = others == 0 ? std::string { "This isn't valid UTF-8." }
                           : "This isn't valid UTF-8, nor are " + std::to_string(others)
            + " more characters in the file, which may be in another encoding.";
    return { "Invalid UTF-8.", Severity::Error, std::vector<Location> { { std::move(text_info), first, msg } } };
}

Diagnostic Diagnostic::unmatched_bracket(std::shared_ptr<const sc::src::TextInfo> text_info,
                                         sc::src::SourceCodeRange close) {
    auto msg = "This " + quoted(text_info->read(close)) + " has no opening bracket.";
    return { "Unmatched Bracket.", Severity::Error, std::vector<Location> { { std::move(text_info), close, msg } } };
}

Diagnostic Diagnostic::mismatched_bracket(std::shared_ptr<const sc::src::TextInfo> text_info,
                                          sc::src::SourceCodeRange open, sc::src::SourceCodeRange close) {
    const auto open_text = text_info->read(open);
    return { "Mismatched Bracket.", //
             Severity::Error, //
             std::vector<Location> {
                 { text_info, open, "This " + quoted(open_text) + "..." },
                 { text_info, close,
                   "...is closed with " + quoted(text_info->read(close)) + ", rather than " + quoted(closing(open_text))
                       + "." },
             } };
}

Diagnostic Diagnostic::unclosed_bracket(std::shared_ptr<const sc::src::TextInfo> text_info,
                                        sc::src::SourceCodeRange open,
                                        std::optional<sc::src::SourceCodeRange> outer_close) {
    const auto open_text = text_info->read(open);
    if (!outer_close)
        return { "Unclosed Bracket.", //
                 Severity::Error, //
                 std::vector<Location> {
                     { text_info, open, "This " + quoted(open_text) + " is never closed." },
                 } };
    return { "Unclosed Bracket.", //
             Severity::Error, //
             std::vector<Location> {
                 { text_info, open, "This " + quoted(open_text) + " isn't closed..." },
                 { text_info, *outer_close,
                   "...before this " + quoted(text_info->read(*outer_close))
                       + ", which closes the bracket around it." },
             } };
}

Diagnostic Diagnostic::unclosed_bracket_by_indentation(std::shared_ptr<const sc::src::TextInfo> text_info,
                                                       sc::src::SourceCodeRange open, sc::src::SourceCodeRange end) {
    const auto open_text = text_info->read(open);
    return { "Unclosed Bracket.", //
             Severity::Error, //
             std::vector<Location> {
                 { text_info, open, "This " + quoted(open_text) + " isn't closed..." },
                 { text_info, end,
                   "...judging by the indentation, it ends here, as the next line is indented no further than the "
                   "line it is on. Add "
                       + quoted(closing(open_text)) + "." },
             } };
}

Diagnostic Diagnostic::missing_operator(std::shared_ptr<const sc::src::TextInfo> text_info,
                                        sc::src::SourceCodeRange before, sc::src::SourceCodeRange found,
                                        std::string_view found_text, std::optional<std::string_view> separator) {
    auto fix = std::string { "Put an operator between them, e.g., `+`" };
    fix += separator ? ", or separate them with " + quoted(*separator) + "." : ".";
    return { "Missing Operator.", //
             Severity::Error, //
             std::vector<Location> {
                 { text_info, before, "This ends an expression..." },
                 { text_info, found, "...and " + std::string { found_text } + " starts another. " + fix },
             } };
}

Diagnostic Diagnostic::missing_expression_after(std::shared_ptr<const sc::src::TextInfo> text_info,
                                                sc::src::SourceCodeRange op, sc::src::SourceCodeRange found,
                                                std::string_view found_text) {
    return { "Missing Expression.", //
             Severity::Error, //
             std::vector<Location> {
                 { text_info, op, "This " + quoted(text_info->read(op)) + " needs an expression after it..." },
                 { text_info, found, "...but found " + std::string { found_text } + "." },
             } };
}

Diagnostic Diagnostic::region_missing_semicolon(std::shared_ptr<const sc::src::TextInfo> text_info,
                                                sc::src::SourceCodeRange loc) {
    return { "Region missing semi-colon.", //
             Severity::Warning, //
             std::vector<Location> { { std::move(text_info), //
                                       loc, //
                                       "Add a semi-colon after this." } } };
}

[[nodiscard]] Diagnostic Diagnostic::duplicate_class(std::shared_ptr<const sc::src::TextInfo> first_text_info,
                                                     sc::src::SourceCodeRange first_loc,
                                                     std::shared_ptr<const sc::src::TextInfo> second_text_info,
                                                     sc::src::SourceCodeRange second_loc) {
    return { "Duplicate Class definition.", //
             Severity::Error, //
             std::vector<Location> {
                 { std::move(first_text_info), first_loc, "Duplicate here..." },
                 { std::move(second_text_info), second_loc, "...and here." },
             } };
}

[[nodiscard]] Diagnostic Diagnostic::todo(std::shared_ptr<const sc::src::TextInfo> info, sc::src::SourceCodeRange loc) {
    return {
        "TODO NOT IMPLEMENTED", //
        Severity::Error, //
        std::vector<Location> {
            { std::move(info), loc, "diagnostic not implemented" } //
        } //
    };
}

[[nodiscard]] Diagnostic Diagnostic::super_class_not_defined(
    std::string name, std::vector<std::pair<std::shared_ptr<const sc::src::TextInfo>, sc::src::SourceCodeRange>> v) {
    std::vector<Location> locs;
    for (auto& [ptr, range] : v) {
        locs.push_back({ ptr, range, "super class not defined." });
    }

    return { "Super class not defined.", //
             Severity::Error, //
             std::move(locs), //
             std::string { "The class " } + name + " was not defined." };
}

[[nodiscard]] Diagnostic Diagnostic::missing_intrinsic_class(std::string extra) {
    return { "Missing intrinsic class", Severity::Error, {}, std::move(extra) };
}

[[nodiscard]] Diagnostic Diagnostic::missing_common_class(std::string extra) {
    return { "Missing common class", Severity::Error, {}, std::move(extra) };
}

[[nodiscard]] Diagnostic Diagnostic::intrinsic_class_wrong_member_name(std::shared_ptr<const sc::src::TextInfo> t,
                                                                       sc::src::SourceCodeRange loc,
                                                                       std::string_view expected) {
    std::stringstream ss;
    ss << "rename to \"" << expected << "\"";
    return { "Intrinsic class has wrong member name.", //
             Severity::Error, //
             std::vector<Location> { { std::move(t), loc, std::move(ss).str() } } };
}


Diagnostic Diagnostic::intrinsic_class_wrong_members(TextInfoPtr t, sc::src::SourceCodeRange loc,
                                                     std::string_view expected) {
    std::string msg = expected.empty() ? std::string { "The VM expects no instance variables." }
                                       : "The VM expects exactly `" + std::string { expected } + "`.";
    return { "Intrinsic class has the wrong instance variables",
             Severity::Error,
             { { std::move(t), loc, std::move(msg) } },
             "Intrinsic classes are a part of the compiler and cannot be changed." };
}

Diagnostic Diagnostic::primitive_argument_count(TextInfoPtr t, sc::src::SourceCodeRange arguments,
                                                sc::src::SourceCodeRange primitive, std::size_t expected,
                                                std::size_t got) {
    // Without the receiver, as they are written.
    const auto count = [](std::size_t n) { return std::to_string(n - 1) + (n == 2 ? " argument" : " arguments"); };
    return { "Primitive argument mismatch",
             Severity::Error,
             {
                 { t, arguments, "The method takes " + count(got) + "..." },
                 { t, primitive, "...but this primitive takes " + count(expected) + "." },
             } };
}

Diagnostic Diagnostic::primitive_needs_variadic_arguments(TextInfoPtr t, sc::src::SourceCodeRange arguments,
                                                          sc::src::SourceCodeRange primitive) {
    return { "Primitive argument mismatch",
             Severity::Error,
             {
                 { t, arguments, "Insert variadic positional arguments, `...args`..." },
                 { t, primitive, "...as this primitive takes them." },
             } };
}

Diagnostic Diagnostic::primitive_needs_keyword_arguments(TextInfoPtr t, sc::src::SourceCodeRange arguments,
                                                         sc::src::SourceCodeRange primitive) {
    return { "Primitive argument mismatch",
             Severity::Error,
             {
                 { t, arguments, "Insert variadic keyword arguments, `...args, kwargs`..." },
                 { t, primitive, "...as this primitive takes them." },
             } };
}

[[nodiscard]] Diagnostic Diagnostic::inheritance_cycle(
    std::vector<std::pair<std::shared_ptr<const sc::src::TextInfo>, sc::src::SourceCodeRange>> m) {
    std::vector<Location> locs {};

    for (auto& [t, r] : m) {
        locs.push_back({ std::move(t), r, "cycle detected" });
    }
    return {
        "Inheritance cycle detected.", //
        Severity::Error, //
        std::move(locs) //
    };
}


[[nodiscard]] Diagnostic Diagnostic::self_inheritance(std::shared_ptr<const sc::src::TextInfo> t,
                                                      sc::src::SourceCodeRange loc) {
    return {
        "Self inheritance",
        Severity::Error,
        {
            { std::move(t), loc, "Remove the super class here, classes cannot inherit from themselves." } //
        } //
    };
}


[[nodiscard]] Diagnostic Diagnostic::duplicate_method_name(TextInfoPtr first_t, sc::src::SourceCodeRange first_l,
                                                           TextInfoPtr second_t, sc::src::SourceCodeRange second_l) {
    return { "Duplicate method name.",
             Severity::Error,
             {
                 // The listing is in source order, so the earlier method, the second one given, reads first.
                 { std::move(second_t), second_l, "This method was duplicated..." },
                 { std::move(first_t), first_l, "...here." },
             } };
}


[[nodiscard]] Diagnostic Diagnostic::duplicate_extention(TextInfoPtr first_t, sc::src::SourceCodeRange first_l,
                                                         TextInfoPtr second_t, sc::src::SourceCodeRange second_l) {
    return { "Duplicate extension",
             Severity::Error,
             {
                 { std::move(first_t), first_l, "This extension defines the method..." },
                 { std::move(second_t), second_l, "...and so does this one, in the same module." },
             },
             "The order of the files in a module isn't defined, so neither can be chosen, and the method can't be "
             "used. Delete one." };
}

[[nodiscard]] Diagnostic Diagnostic::extension_conflict(TextInfoPtr first_t, sc::src::SourceCodeRange first_l,
                                                        TextInfoPtr second_t, sc::src::SourceCodeRange second_l) {
    return { "Extensions conflict",
             Severity::Error,
             {
                 { std::move(first_t), first_l, "This extension defines the method..." },
                 { std::move(second_t), second_l, "...and so does this one, in another module." },
             },
             "From 3.16, extensions in different modules can't define the same method, as which is used would depend "
             "on the order the modules are given, so the method can't be used. Delete or rename one." };
}

[[nodiscard]] Diagnostic Diagnostic::extension_overwritten(TextInfoPtr first_t, sc::src::SourceCodeRange first_l,
                                                           TextInfoPtr second_t, sc::src::SourceCodeRange second_l) {
    return { "Extension overwritten",
             Severity::Warning,
             {
                 { std::move(first_t), first_l, "This extension defines the method..." },
                 { std::move(second_t), second_l, "...but is replaced by this one, from a later module." },
             },
             "In the old compiler the extension from the module given last is used. From "
             "3.16 this is an error, as which is used depends on the order the modules are given." };
}
[[nodiscard]] Diagnostic Diagnostic::extension_of_undefined_class(TextInfoPtr t, sc::src::SourceCodeRange l) {
    return { "Extension of an undefined class",
             Severity::Warning,
             { { std::move(t), l, "No class with this name is defined." } },
             "The extension is skipped. Check the name, or that the file defining the class is given to the "
             "compiler." };
}
[[nodiscard]] Diagnostic Diagnostic::default_inheritor_must_have_super(TextInfoPtr first_t,
                                                                       sc::src::SourceCodeRange first_l) {
    return { "Default inheritor must inherit from a class.",
             Severity::Error,
             {
                 { std::move(first_t), first_l, "Should inherit from something." },
             },
             "The default inheritor class needs a specified super class (unless it is also the root object)." };
}


[[nodiscard]] Diagnostic Diagnostic::root_object_must_not_inherit(TextInfoPtr first_t,
                                                                  sc::src::SourceCodeRange first_l) {
    return { "Root object must not inherit from something.",
             Severity::Error,
             {
                 { std::move(first_t), first_l, "Remove this super class." },
             },
             "The root object is the highest node in the class heirachy, it should not inherit from another class or "
             "there will be a cycle." };
}

Diagnostic Diagnostic::unexpected_slot_def(TextInfoPtr first_t, sc::src::SourceCodeRange first_l) {
    return { "Invalid slot def",
             Severity::Error,
             {
                 { std::move(first_t), first_l,
                   "This is an internal implementation detail, remove this, or consult the interpreter source code." },
             },
             "Slot defs are an internal implementation detail of intrinsic classes, you should not use it unless you "
             "are writing a core part of the interpreter. They are slot, double, float, int32, int16, int8, char and "
             "symbol." };
}


Diagnostic Diagnostic::incorrect_super_class(TextInfoPtr first_t, sc::src::SourceCodeRange first_l,
                                             std::string_view expected) {
    std::string msg;
    msg += "Should be '";
    msg += expected;
    msg += "'.";
    return { "Intrinsic has incorrect super class",
             Severity::Error,
             {
                 { std::move(first_t), first_l, std::move(msg) },
             },
             "Intrinsic classes are a part of the compiler and cannot be changed." };
}

Diagnostic Diagnostic::incorrect_super_class(TextInfoPtr first_t, sc::src::SourceCodeRange first_l) {
    return { "Intrinsic has unexpected super class",
             Severity::Error,
             {
                 { std::move(first_t), first_l, "Delete this." },
             },
             "Intrinsic classes are a part of the compiler and cannot be changed." };
}

Diagnostic Diagnostic::incorrect_slot_def(TextInfoPtr first_t, sc::src::SourceCodeRange first_l,
                                          std::string_view expected) {
    std::string msg;
    msg += "Should be '";
    msg += expected;
    msg += "'.";
    return { "Intrinsic has incorrect slot def.",
             Severity::Error,
             {
                 { std::move(first_t), first_l, std::move(msg) },
             },
             "Intrinsic classes are a part of the compiler and cannot be changed." };
}
Diagnostic Diagnostic::incorrect_slot_def(TextInfoPtr first_t, sc::src::SourceCodeRange first_l) {
    return { "Intrinsic has slot def when shouldn't",
             Severity::Error,
             {
                 { std::move(first_t), first_l, "Delete this." },
             },
             "Intrinsic classes are a part of the compiler and cannot be changed." };
}


Diagnostic Diagnostic::duplicate_member(TextInfoPtr first_t, sc::src::SourceCodeRange first_l, TextInfoPtr second_t,
                                        sc::src::SourceCodeRange second_l) {
    return { "Duplicate member in class",
             Severity::Error,
             {
                 { std::move(first_t), first_l, "This member is declared here..." },
                 { std::move(second_t), second_l, "...and again here." },
             },
             "Duplicate members are not allowed in a class definition. Please delete one." };
}


Diagnostic Diagnostic::duplicate_member_in_base(TextInfoPtr first_t, sc::src::SourceCodeRange first_l,
                                                TextInfoPtr second_t, sc::src::SourceCodeRange second_l) {
    return { "Duplicate member with base class.",
             Severity::Error,
             {
                 { std::move(first_t), first_l, "First declared here..." },
                 { std::move(second_t), second_l, " ...duplicated here." },
             },
             "Duplicate members are not allowed within a class hierarchy. Please delete one." };
}

Diagnostic Diagnostic::hidden_class_member(TextInfoPtr first_t, sc::src::SourceCodeRange first_l, TextInfoPtr second_t,
                                           sc::src::SourceCodeRange second_l) {
    return { "Class member hides the superclass's",
             Severity::Warning,
             {
                 { std::move(first_t), first_l, "Declared in a superclass here..." },
                 { std::move(second_t), second_l, "...and again here, which hides it." },
             },
             "This class and its subclasses use this one, the superclass's own methods still use theirs. Rename one "
             "unless that is what you meant." };
}

Diagnostic Diagnostic::non_terminating_return(TextInfoPtr first_t, sc::src::SourceCodeRange first_l) {
    return { "Return is not final expression",
             Severity::Error,
             { {
                 std::move(first_t),
                 first_l,
             } },
             "Returns should be the final expression in a function." };
}

Diagnostic Diagnostic::undefined_variable(TextInfoPtr first_t, sc::src::SourceCodeRange first_l,
                                          std::string_view name) {
    return { "Variable not defined",
             Severity::Error,
             { { std::move(first_t), first_l, "this is not defined." } },
             "Declare it with 'var', or use '~" + std::string { name } + "' to refer to an environment variable." };
}

Diagnostic Diagnostic::message_without_receiver(TextInfoPtr first_t, sc::src::SourceCodeRange first_l) {
    return { "Message has no receiver",
             Severity::Error,
             { { std::move(first_t), first_l, "there is nothing to send this message to." } },
             "This message has no receiver. Either write 'a.foo', 'foo(a)', or a 'foo: b'." };
}

Diagnostic Diagnostic::not_constant(TextInfoPtr first_t, sc::src::SourceCodeRange first_l) {
    return { "Not a constant",
             Severity::Error,
             { { std::move(first_t), first_l, "this can't be evaluated when compiling." } },
             "This is required to be a constant, this is the usual literals and a few compiler known operations." };
}

Diagnostic Diagnostic::nested_too_deeply(TextInfoPtr t, sc::src::SourceCodeRange l, std::size_t limit) {
    return { "Nested too deeply",
             Severity::Error,
             { { std::move(t), l, "this is nested more than " + std::to_string(limit) + " deep." } },
             "The compiler can't compile code nested this deeply, e.g., brackets inside brackets, or a long chain of "
             "binary operators, `a + b + c ...`, which is as deep as it is long. Split it up, e.g., into variables." };
}

Diagnostic Diagnostic::too_many_variables(TextInfoPtr t, sc::src::SourceCodeRange l, std::size_t count,
                                          std::size_t limit) {
    return { "Too many variables",
             Severity::Error,
             { { std::move(t), l,
                 "this has " + std::to_string(count) + " arguments and variables, more than " + std::to_string(limit)
                     + "." } },
             "A method or function can have at most " + std::to_string(limit)
                 + " arguments and variables, counting `this` for a method. Group them, e.g., into an Array or an "
                   "Event, or split the code into smaller functions." };
}

Diagnostic Diagnostic::instance_variable_out_of_reach(TextInfoPtr t, sc::src::SourceCodeRange l, std::size_t index,
                                                      std::size_t limit) {
    return { "Instance variable out of reach",
             Severity::Error,
             { { std::move(t), l,
                 "this uses instance variable number " + std::to_string(index) + ", past number "
                     + std::to_string(limit) + "." } },
             "Bytecode refers to an instance variable by a single byte, so only the first " + std::to_string(limit + 1)
                 + ", counting those of the superclasses, can be used by a method or function written in a class. "
                   "Group the others, e.g., into an Event." };
}

Diagnostic Diagnostic::class_variable_out_of_reach(TextInfoPtr t, sc::src::SourceCodeRange l, std::size_t index,
                                                   std::size_t limit) {
    return { "Class variable out of reach",
             Severity::Error,
             { { std::move(t), l,
                 "this uses class variable number " + std::to_string(index) + ", past number " + std::to_string(limit)
                     + "." } },
             "Bytecode refers to a class variable by two bytes, so only the first " + std::to_string(limit + 1)
                 + " class variables of all the classes together can be used. Group some, e.g., into an Event." };
}

Diagnostic Diagnostic::selector_table_full(TextInfoPtr t, sc::src::SourceCodeRange l, std::size_t limit) {
    return { "Too many selectors",
             Severity::Error,
             { { std::move(t), l,
                 "this uses more than " + std::to_string(limit)
                     + " different selectors, class names and functions." } },
             "Split the code into smaller functions." };
}

Diagnostic Diagnostic::jump_too_long(TextInfoPtr t, sc::src::SourceCodeRange l, std::size_t length, std::size_t limit) {
    return { "Code too long",
             Severity::Error,
             { { std::move(t), l,
                 "this has " + std::to_string(length) + " bytes of bytecode to jump over, more than "
                     + std::to_string(limit) + "." } },
             "The VM can jump over at most " + std::to_string(limit)
                 + " bytes of bytecode. Move the code into a "
                   "method or function." };
}

Diagnostic Diagnostic::too_many_arguments(TextInfoPtr t, sc::src::SourceCodeRange l, std::size_t count,
                                          std::size_t limit) {
    return { "Too many arguments",
             Severity::Error,
             { { std::move(t), l,
                 "a message here is sent with " + std::to_string(count) + " arguments, more than "
                     + std::to_string(limit) + "." } },
             "A message can be sent with at most " + std::to_string(limit)
                 + " arguments, counting the receiver, and two for each keyword argument. Pass them in an Array or an "
                   "Event, or use performList." };
}

Diagnostic Diagnostic::hard_coded_loop_signature(TextInfoPtr t, sc::src::SourceCodeRange l, std::size_t arguments,
                                                 std::size_t variables) {
    return { "Hard coded method has the wrong arguments or variables",
             Severity::Error,
             { { std::move(t), l,
                 "this method's code is replaced by a loop that expects " + std::to_string(arguments)
                     + " arguments, counting `this`, and " + std::to_string(variables)
                     + " variables, and no variadic arguments." } },
             "The compiler replaces the code of some loop methods, e.g., `Integer:do` and `ArrayedCollection:do`, with "
             "special bytecode that uses the method's arguments and variables directly, so they must be declared as "
             "in the class library." };
}

Diagnostic Diagnostic::hard_coded_loop_extension_ignored(TextInfoPtr t, sc::src::SourceCodeRange l) {
    return { "Extension of a hard coded method is ignored",
             Severity::Warning,
             { { std::move(t), l, "this extension's code is replaced by a loop." } },
             "The compiler replaces the code of some loop methods, e.g., `Integer:do` and `ArrayedCollection:do`, with "
             "special bytecode. Before 3.16 this also happens to an extension method replacing one of them, so the "
             "extension's code never runs. Add `//# VERSION 3.16` to the extension to fix this." };
}

Diagnostic Diagnostic::hard_coded_loop_computed_value(TextInfoPtr t, sc::src::SourceCodeRange l) {
    return { "Hard coded method with a computed value",
             Severity::Error,
             { { std::move(t), l, "this would never be computed, as the method's code is replaced by a loop." } },
             "The compiler replaces the code of some loop methods, e.g., `Integer:do` and `ArrayedCollection:do`, with "
             "special bytecode, so their arguments' defaults and variables' initial values can only be literals." };
}

Diagnostic Diagnostic::literal_out_of_range(TextInfoPtr t, sc::src::SourceCodeRange l, bool is_float) {
    if (is_float)
        return { "Literal out of range",
                 Severity::Error,
                 { { std::move(t), l, "this Float is too large or too small to be represented." } },
                 "Floats are 64 bit, so other than zero, a Float literal's size must be between about 4.9e-324 and "
                 "1.8e308. Use `inf` for infinity." };
    return { "Literal out of range",
             Severity::Error,
             { { std::move(t), l, "this is outside the range of a 32 bit Integer." } },
             "Integers are 32 bit, from -2147483648 to 2147483647. "
             "Use a Float for larger numbers, e.g.,`2147483648.0`." };
}

Diagnostic Diagnostic::variadic_argument_not_last(TextInfoPtr variadic_t, sc::src::SourceCodeRange variadic_l,
                                                  TextInfoPtr positional_t, sc::src::SourceCodeRange positional_l) {
    return { "Variadic argument is not last",
             Severity::Error,
             {
                 { std::move(variadic_t), variadic_l, "the elements of this are passed after..." },
                 { std::move(positional_t), positional_l, "...this argument, which is written after it." },
             },
             "A variadic argument, e.g., `*a`, must come after all the positional arguments, e.g., `foo(1, *a)`. "
             "This includes the value of `a[*i] = v`, which is passed after the 'v'." };
}

Diagnostic Diagnostic::event_key_without_value(TextInfoPtr first_t, sc::src::SourceCodeRange first_l) {
    return { "Event key without a value",
             Severity::Warning,
             { { std::move(first_t), first_l, "this key has no value." } },
             "The elements of `Event[...]` are pairs of keys and values, e.g., `Event[a: 1, b: 2]`. Add a value, or "
             "remove the key." };
}

Diagnostic Diagnostic::unbounded_range(TextInfoPtr first_t, sc::src::SourceCodeRange first_l) {
    return { "Range has no end",
             Severity::Error,
             { { std::move(first_t), first_l, "this would create an infinite array." } },
             "A range without an end, e.g., `(1..)`, can only be iterated, e.g., in a generator. Add an end, e.g., "
             "`(1..10)`. To iterate forever, as `(1..).do(f)` did before 3.16, write `(:1..)`." };
}

Diagnostic Diagnostic::unbounded_range_do(TextInfoPtr t, sc::src::SourceCodeRange l) {
    return { "Iterating a range without an end is removed in 3.16",
             Severity::Warning,
             { { std::move(t), l, "this counts up forever." } },
             "Before 3.16, `(1..).do(f)` calls f with each number from 1 forever, without making the range. From "
             "3.16 a range without an end is an error outside a generator. Try `do((:1..)) {...}`." };
}

Diagnostic Diagnostic::range_too_many_arguments(TextInfoPtr first_t, sc::src::SourceCodeRange first_l) {
    return { "Too many arguments in range",
             Severity::Error,
             { { std::move(first_t), first_l, "only the first, and optionally the second, can come before `..`." } },
             "A range is `first..last`, or `first, second..last` where second sets the step, e.g., `x[1, 3..9]`." };
}

Diagnostic Diagnostic::range_argument_not_value(TextInfoPtr first_t, sc::src::SourceCodeRange first_l) {
    return { "Range argument is not a value",
             Severity::Error,
             { { std::move(first_t), first_l, "keyword and variadic arguments can't be used in a range." } },
             "A range is `first..last`, or `first, second..last` where second sets the step, e.g., `x[1, 3..9]`." };
}

Diagnostic Diagnostic::duplicate_variable(TextInfoPtr first_t, sc::src::SourceCodeRange first_l, TextInfoPtr second_t,
                                          sc::src::SourceCodeRange second_l) {
    return { "Variable already declared",
             Severity::Error,
             {
                 { std::move(first_t), first_l, "First declared here..." },
                 { std::move(second_t), second_l, "...declared again here." },
             },
             "Arguments and variables in the same function or method must have different names. Rename one." };
}

Diagnostic Diagnostic::declared_environment_variable(TextInfoPtr t, sc::src::SourceCodeRange l) {
    return { "Environment variable declared",
             Severity::Error,
             { { std::move(t), l, "this can't be declared." } },
             "Only local variables can be declared with 'var'. Remove the '~' to declare a variable, or assign the "
             "environment variable after the declaration." };
}

Diagnostic Diagnostic::missing_interpreter_context(TextInfoPtr t, sc::src::SourceCodeRange l) {
    return {
        "Interpreter context missing",
        Severity::Error,
        { { std::move(t), l, "this code can't be compiled without it." } },
        "Interpreted code is compiled as if it were written inside the method `Interpreter:functionCompileContext`, "
        "so it can use the Interpreter's members, e.g., `a` to `z`. The class library must define it, and it must "
        "compile without errors."
    };
}

Diagnostic Diagnostic::missing_interpreter_class(TextInfoPtr t, sc::src::SourceCodeRange l) {
    return {
        "Interpreter missing",
        Severity::Error,
        { { std::move(t), l, "this closed function can't be compiled without it." } },
        "A closed function, `#{ }`, is compiled as if it were written inside the Interpreter, so it can use the "
        "Interpreter's members, e.g., `a` to `z`, but not the variables around it. The class library must define the "
        "Interpreter, and it must compile without errors."
    };
}

Diagnostic Diagnostic::declaration_after_expression(TextInfoPtr t, sc::src::SourceCodeRange l) {
    return { "Variable declared after an expression",
             Severity::Error,
             { { std::move(t), l, "this isn't at the start of the body." } },
             "In the legacy version, as in the old compiler, a function's or method's variables must be declared at "
             "the start of its body, before any other expression. From 3.16 they can be declared anywhere. Move the "
             "declaration to the start, or add `//# VERSION 3.16`." };
}

Diagnostic Diagnostic::return_in_closed_function(TextInfoPtr t, sc::src::SourceCodeRange l, bool legacy) {
    const std::string why = "`^` returns from the method the code is written in, but a closed function, `#{ }`, isn't "
                            "in one, even when it is written in a method";
    const std::string fix =
        "A function returns its last expression, so remove the `^`, or use `{ }` instead of `#{ }`.";
    return { "Return in closed function",
             legacy ? Severity::Warning : Severity::Error,
             { { std::move(t), l, "there is no method to return from." } },
             legacy ? why + ", so this fails when it runs. From 3.16 it is an error. " + fix : why + ". " + fix };
}

Diagnostic Diagnostic::outside_closed_function(TextInfoPtr declaration_t, sc::src::SourceCodeRange declaration,
                                               TextInfoPtr use_t, sc::src::SourceCodeRange use) {
    return { "Variable outside closed function",
             Severity::Error,
             { { std::move(declaration_t), declaration, "This is declared outside the closed function..." },
               { std::move(use_t), use, "...so it can't be used inside it." } },
             "A closed function, `#{ }`, is compiled as if it were written inside the Interpreter, so it can only use "
             "its own arguments and variables, and the Interpreter's members, e.g., `a` to `z`. Pass the value in as "
             "an argument, or use `{ }` instead of `#{ }`." };
}

Diagnostic Diagnostic::return_in_interpreted_code(TextInfoPtr t, sc::src::SourceCodeRange l) {
    return { "Return in interpreted code",
             Severity::Error,
             { { std::move(t), l, "there is no method to return from." } },
             "`^` returns from the method the code is written in, but interpreted code isn't in a method. "
             "The value of interpreted code is its last expression, so remove the `^`." };
}

Diagnostic Diagnostic::qualifier_hidden_from_value(TextInfoPtr declaration_t, sc::src::SourceCodeRange declaration,
                                                   TextInfoPtr use_t, sc::src::SourceCodeRange use) {
    return { "Qualifier's variable used in its own value",
             Severity::Warning,
             { { std::move(declaration_t), declaration, "This qualifier declares it..." },
               { std::move(use_t), use, "...but this function in its value can't see it." } },
             "Rename one of them to keep this meaning." };
}

Diagnostic Diagnostic::malformed_version(TextInfoPtr t, sc::src::SourceCodeRange l) {
    return { "Malformed version",
             Severity::Error,
             { { std::move(t), l, "expected a version, e.g., `//# VERSION 3.16`, or `//# VERSION legacy`." } },
             "The classes it applies to are compiled as the legacy version." };
}

Diagnostic Diagnostic::unknown_version(TextInfoPtr t, sc::src::SourceCodeRange l, std::string_view latest) {
    return { "Unknown version",
             Severity::Error,
             { { std::move(t), l, "this version is newer than the compiler." } },
             "The latest version is " + std::string { latest }
                 + ". The classes it applies to are compiled as the legacy version." };
}

Diagnostic Diagnostic::duplicate_version(TextInfoPtr t, sc::src::SourceCodeRange first,
                                         sc::src::SourceCodeRange second) {
    return { "Duplicate version",
             Severity::Warning,
             { { t, first, "This version is ignored..." }, { t, second, "...as this one is used." } } };
}

Diagnostic Diagnostic::misplaced_version(TextInfoPtr t, sc::src::SourceCodeRange l) {
    return { "Misplaced version",
             Severity::Warning,
             { { std::move(t), l, "this version applies to nothing." } },
             "A version must be the first thing in the file, for every class in it, or the first thing in a class's "
             "body, for that class." };
}

Diagnostic Diagnostic::version_in_interpreted_code(TextInfoPtr t, sc::src::SourceCodeRange l) {
    return { "Version in interpreted code",
             Severity::Warning,
             { { std::move(t), l, "this version is ignored." } },
             "Interpreted code is given its version when it is compiled." };
}

Diagnostic Diagnostic::curry_outside_call(TextInfoPtr t, sc::src::SourceCodeRange l) {
    return {
        "Partial application outside of a call",
        Severity::Error,
        { { std::move(t), l, "this isn't an argument of a call." } },
        "`_` makes a function from the call it is in, e.g., `_ + 1` is `{ |x| x + 1 }`. It must be the receiver or an "
        "argument of a message, or an element of an array or dictionary, not inside another expression."
    };
}

namespace {
constexpr auto special_variables =
    "`this`, `super`, `thisProcess`, `thisThread`, `thisMethod`, `thisFunctionDef` and `thisFunction` are set by the "
    "language";
}

Diagnostic Diagnostic::assigned_special_variable(TextInfoPtr t, sc::src::SourceCodeRange l) {
    return { "Special variable assigned",
             Severity::Error,
             { { std::move(t), l, "this can't be assigned." } },
             std::string { special_variables } + ", so can't be assigned." };
}

Diagnostic Diagnostic::not_compiled(TextInfoPtr t, sc::src::SourceCodeRange l) {
    return { "Not compiled",
             Severity::Error,
             { { std::move(t), l, "the compiler can't compile this yet." } },
             "The backend doesn't support something in this code yet, so calling it throws a PoisonedMethodError." };
}

Diagnostic Diagnostic::unsupported_environment_pack(TextInfoPtr t, sc::src::SourceCodeRange l) {
    return { "Unsupported assignment",
             Severity::Error,
             { { std::move(t), l, "environment variables can't be packed yet." } },
             "Assigning several values at once to environment variables, e.g., `#~a, ~b = x`, isn't supported yet. "
             "Assign them one at a time, e.g., `~a = x[0]; ~b = x[1]`." };
}

Diagnostic Diagnostic::assigned_constant(TextInfoPtr const_t, sc::src::SourceCodeRange const_l, TextInfoPtr assign_t,
                                         sc::src::SourceCodeRange assign_l) {
    return { "Assigning to a constant",
             Severity::Error,
             {
                 { std::move(const_t), const_l, "Declared as a constant here..." },
                 { std::move(assign_t), assign_l, "...but assigned here." },
             },
             "A constant's value can't change. Declare it with `classvar` if it needs to be assigned." };
}

Diagnostic Diagnostic::constant_setter(TextInfoPtr t, sc::src::SourceCodeRange l) {
    return { "Constant with a setter",
             Severity::Error,
             { { std::move(t), l, "this makes a setter." } },
             "A constant can't be assigned, so it can only have a getter, `<`. The setter is left out." };
}

Diagnostic Diagnostic::declared_special_variable(TextInfoPtr t, sc::src::SourceCodeRange l) {
    return { "Special variable declared",
             Severity::Error,
             { { std::move(t), l, "this name is reserved." } },
             std::string { special_variables } + ". Use a different name." };
}
}
