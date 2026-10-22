#include "sc_diagnostic/sc_diagnostic.hpp"
#include "sc_lexer/source_utils.hpp"
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

    size_t i { 0 };
    for (const auto& loc : locations) {
        const auto filepath = loc.text_info->file_path;
        const auto txt = loc.text_info->source.as_string().c_str();
        const auto txt_len = loc.text_info->source.as_string().size();
        const auto range = loc.range;
        const auto source_line = loc.text_info->source_start_in_file.line_number;
        const auto source_column = loc.text_info->source_start_in_file.column;
        const auto& description = loc.msg;

        const auto starting_line = source_line + range.begin.line_number;
        // Where the range starts, counting from zero. The text's start column only applies to its first line.
        const auto start_column = (range.begin.line_number == 0 ? source_column : 0) + range.begin.column;

        if (filepath && relative_to) {
            const auto relative =
                std::filesystem::absolute(*filepath).lexically_relative(std::filesystem::absolute(*relative_to));
            ss << relative.generic_string() << ':' << starting_line + 1 << ':' << start_column + 1 << '\n';
        } else if (filepath) {
            const auto abs = std::filesystem::absolute(*filepath);
            ss << "file://" << abs.string() << ':' << starting_line + 1 << ':' << start_column + 1 << '\n';
        } else
            ss << "unknown file:" << starting_line + 1 << ':' << start_column + 1 << '\n';

        if (!txt || txt_len == 0) {
            ss << description;
            break; // nothing left to do
        }
        const auto selection_start = txt + range.begin.absolute;
        const auto selection_end = txt + range.end.absolute;

        auto maybe_iter = sc::lex::utils::LineIter::make(txt, txt + txt_len, range.begin);
        if (!maybe_iter.has_value())
            continue;

        auto iter = std::move(*maybe_iter);

        if (range.line_count() <= 4) {
            iter.backwards();
            iter.backwards();
        }

        const auto single_line_error = range.line_count() == 1;

        for (auto line = iter.forwards(); line; line = iter.forwards()) {
            const auto [ptr, sz, line_number, ends_in_newline_char] = *line;
            const auto real_line_number = line_number + 1 + source_line;

            // If is outside of the highlight range
            if (line_number < range.begin.line_number || line_number > range.end.line_number) {
                ss << std::setfill(' ') << std::setw(5) << real_line_number << " │ ";
                ss.write(ptr, sz);
                if (!ends_in_newline_char)
                    ss << '\n';
                if (line_number > range.end.line_number + 2)
                    break;
                else
                    continue;
            }
            // Otherwise...

            if (single_line_error) {
                ss << std::setfill(' ') << std::setw(5) << real_line_number << " │ ";
                ss.write(ptr, sz);

                if (!ends_in_newline_char)
                    ss << '\n';

                ss << "      ┆ ";
                auto cp_iter = sc::lex::utils::CodePointIterator::make(ptr, ptr + sz, ptr).value();

                for (auto cp = cp_iter.forwards(); cp; cp = cp_iter.forwards()) {
                    if (cp_iter.current_location() <= selection_start) {
                        ss << sc::lex::codepoint_as_whitespace(*cp);
                    } else if (cp_iter.current_location() <= selection_end) {
                        const auto w { sc::lex::codepoint_width(*cp) };
                        for (size_t i { 0 }; i < w; ++i)
                            ss << '^';
                    } else
                        break;
                }

                ss << " " << description << '\n';
            } else {
                ss << std::setfill(' ') << std::setw(5) << real_line_number << "*│ ";
                ss.write(ptr, sz);

                if (!ends_in_newline_char)
                    ss << '\n';

                if (line_number == range.end.line_number) {
                    ss << "     >│ ^ " << description << '\n';
                }
            }
        }

        if (i < locations.size() - 1) {
            ss << "\n";
        }

        ++i;
    }

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
    m_severity(severity) { }

const std::vector<Diagnostic::Location>& Diagnostic::locations() const { return m_locations; }

Severity Diagnostic::severity() const { return m_severity; }

bool Diagnostic::fatal() const { return m_severity == Severity::Error; }

[[nodiscard]] const char* Diagnostic::name() const { return m_name; }

std::string Diagnostic::to_string() const { return diagnosticToString(severity(), name(), locations(), extra_info); }

std::string Diagnostic::to_string(const std::filesystem::path& relative_to) const {
    return diagnosticToString(severity(), name(), locations(), extra_info, &relative_to);
}

////////////////////////////////////////////////////////////////////////////////

Diagnostic Diagnostic::unexpectedToken(std::shared_ptr<const sc::lex::TextInfo> text_info, sc::lex::SourceCodeRange loc,
                                       std::string expected, const char* received) {
    return { "Unexpected Token.", //
             Severity::Error, //
             std::vector<Location> { {
                 std::move(text_info), //
                 loc, //
                 std::string { "Expected: " } + std::move(expected) + " but received " + received //
             } } };
};

Diagnostic Diagnostic::region_missing_semicolon(std::shared_ptr<const sc::lex::TextInfo> text_info,
                                                sc::lex::SourceCodeRange loc) {
    return { "Region missing semi-colon.", //
             Severity::Error, //
             std::vector<Location> { { std::move(text_info), //
                                       loc, //
                                       "Insert semi-colon here." } } };
}

[[nodiscard]] Diagnostic Diagnostic::duplicate_class(std::shared_ptr<const sc::lex::TextInfo> first_text_info,
                                                     sc::lex::SourceCodeRange first_loc,
                                                     std::shared_ptr<const sc::lex::TextInfo> second_text_info,
                                                     sc::lex::SourceCodeRange second_loc) {
    return { "Duplicate Class definition.", //
             Severity::Error, //
             std::vector<Location> {
                 { std::move(first_text_info), first_loc, "Duplicate here..." },
                 { std::move(second_text_info), second_loc, "...and here." },
             } };
}

[[nodiscard]] Diagnostic Diagnostic::todo(std::shared_ptr<const sc::lex::TextInfo> info, sc::lex::SourceCodeRange loc) {
    return {
        "TODO NOT IMPLEMENTED", //
        Severity::Error, //
        std::vector<Location> {
            { std::move(info), loc, "diagnostic not implemented" } //
        } //
    };
}

[[nodiscard]] Diagnostic Diagnostic::super_class_not_defined(
    std::string name, std::vector<std::pair<std::shared_ptr<const sc::lex::TextInfo>, sc::lex::SourceCodeRange>> v) {
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
    return { "Missing intrinsic class", Severity::Error, { }, std::move(extra) };
}

[[nodiscard]] Diagnostic Diagnostic::missing_common_class(std::string extra) {
    return { "Missing common class", Severity::Error, { }, std::move(extra) };
}

[[nodiscard]] Diagnostic Diagnostic::intrinsic_class_wrong_member_name(std::shared_ptr<const sc::lex::TextInfo> t,
                                                                       sc::lex::SourceCodeRange loc,
                                                                       std::string_view expected) {
    std::stringstream ss;
    ss << "rename to \"" << expected << "\"";
    return { "Intrinsic class has wrong member name.", //
             Severity::Error, //
             std::vector<Location> { { std::move(t), loc, std::move(ss).str() } } };
}


[[nodiscard]] Diagnostic Diagnostic::inheritance_cycle(
    std::vector<std::pair<std::shared_ptr<const sc::lex::TextInfo>, sc::lex::SourceCodeRange>> m) {
    std::vector<Location> locs { };

    for (auto& [t, r] : m) {
        locs.push_back({ std::move(t), r, "cycle detected" });
    }
    return {
        "Inheritance cycle detected.", //
        Severity::Error, //
        std::move(locs) //
    };
}


[[nodiscard]] Diagnostic Diagnostic::self_inheritance(std::shared_ptr<const sc::lex::TextInfo> t,
                                                      sc::lex::SourceCodeRange loc) {
    return {
        "Self inheritance",
        Severity::Error,
        {
            { std::move(t), loc, "Remove the super class here, classes cannot inherit from themselves." } //
        } //
    };
}


[[nodiscard]] Diagnostic Diagnostic::duplicate_method_name(TextInfoPtr first_t, sc::lex::SourceCodeRange first_l,
                                                           TextInfoPtr second_t, sc::lex::SourceCodeRange second_l) {
    return { "Duplicate method name.",
             Severity::Error,
             {
                 { std::move(first_t), first_l, "This method was duplicated..." },
                 { std::move(second_t), second_l, "...here." },
             } };
}


[[nodiscard]] Diagnostic Diagnostic::duplicate_extention(TextInfoPtr first_t, sc::lex::SourceCodeRange first_l,
                                                         TextInfoPtr second_t, sc::lex::SourceCodeRange second_l) {
    return { "Duplicate extention.",
             Severity::Error,
             {
                 { std::move(first_t), first_l, "Overrideen here..." },
                 { std::move(second_t), second_l, "... and also overrideen here." },
             },
             "The compile cannot determine which should implementation should be used, please delete one." };
}
[[nodiscard]] Diagnostic Diagnostic::default_inheritor_must_have_super(TextInfoPtr first_t,
                                                                       sc::lex::SourceCodeRange first_l) {
    return { "Default inheritor must inherit from a class.",
             Severity::Error,
             {
                 { std::move(first_t), first_l, "Should inherit from something." },
             },
             "The default inheritor class needs a specified super class (unless it is also the root object)." };
}


[[nodiscard]] Diagnostic Diagnostic::root_object_must_not_inherit(TextInfoPtr first_t,
                                                                  sc::lex::SourceCodeRange first_l) {
    return { "Root object must not inherit from something.",
             Severity::Error,
             {
                 { std::move(first_t), first_l, "Remove this super class." },
             },
             "The root objec is the highest node in the class heirachy, it should not inherit from another class or "
             "there will be a cycle." };
}

Diagnostic Diagnostic::unexpected_slot_def(TextInfoPtr first_t, sc::lex::SourceCodeRange first_l) {
    return { "Invalid slot def",
             Severity::Error,
             {
                 { std::move(first_t), first_l,
                   "This is an internal implementation detail, remove this, or consult the interpreter source code." },
             },
             "Slot defs are an internal implementation detail of intrinsic classes, you should not use it unless you "
             "are writing a core part of the interpreter." };
}


Diagnostic Diagnostic::incorrect_super_class(TextInfoPtr first_t, sc::lex::SourceCodeRange first_l,
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

Diagnostic Diagnostic::incorrect_super_class(TextInfoPtr first_t, sc::lex::SourceCodeRange first_l) {
    return { "Intrinsic has unexpected super class",
             Severity::Error,
             {
                 { std::move(first_t), first_l, "Delete this." },
             },
             "Intrinsic classes are a part of the compiler and cannot be changed." };
}

Diagnostic Diagnostic::incorrect_slot_def(TextInfoPtr first_t, sc::lex::SourceCodeRange first_l,
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
Diagnostic Diagnostic::incorrect_slot_def(TextInfoPtr first_t, sc::lex::SourceCodeRange first_l) {
    return { "Intrinsic has slot def when shouldn't",
             Severity::Error,
             {
                 { std::move(first_t), first_l, "Delete this." },
             },
             "Intrinsic classes are a part of the compiler and cannot be changed." };
}


Diagnostic Diagnostic::duplicate_member(TextInfoPtr first_t, sc::lex::SourceCodeRange first_l, TextInfoPtr second_t,
                                        sc::lex::SourceCodeRange second_l) {
    return { "Duplicate member in class",
             Severity::Error,
             {
                 { std::move(first_t), first_l },
                 { std::move(second_t), second_l },
             },
             "Duplicate members are not allowed in a class definition. Please delete one." };
}


Diagnostic Diagnostic::duplicate_member_in_base(TextInfoPtr first_t, sc::lex::SourceCodeRange first_l,
                                                TextInfoPtr second_t, sc::lex::SourceCodeRange second_l) {
    return { "Duplicate member with base class.",
             Severity::Error,
             {
                 { std::move(first_t), first_l, "First declared here..." },
                 { std::move(second_t), second_l, " ...duplicated here." },
             },
             "Duplicate members are not allowed within a class hierarchy. Please delete one." };
}

Diagnostic Diagnostic::non_terminating_return(TextInfoPtr first_t, sc::lex::SourceCodeRange first_l) {
    return { "Return is not final expression",
             Severity::Error,
             { {
                 std::move(first_t),
                 first_l,
             } },
             "Returns should be the final expression in a function." };
}

Diagnostic Diagnostic::undefined_variable(TextInfoPtr first_t, sc::lex::SourceCodeRange first_l,
                                          std::string_view name) {
    return { "Variable not defined",
             Severity::Error,
             { { std::move(first_t), first_l, "this is not defined." } },
             "Declare it with 'var', or use '~" + std::string { name } + "' to refer to an environment variable." };
}

Diagnostic Diagnostic::message_without_receiver(TextInfoPtr first_t, sc::lex::SourceCodeRange first_l) {
    return { "Message has no receiver",
             Severity::Error,
             { { std::move(first_t), first_l, "there is nothing to send this message to." } },
             "Functional notation sends the message to its first "
             "argument, `foo(a, b)` is the same as `a.foo(b)`. "
             "Add a positional argument, e.g., `foo(a)`, or use "
             "`a.foo`." };
}

Diagnostic Diagnostic::perform_selector_not_symbol(TextInfoPtr first_t, sc::lex::SourceCodeRange first_l) {
    return { "Perform selector is not a symbol",
             Severity::Error,
             { { std::move(first_t), first_l, "this is a constant, but not a symbol." } },
             "`perform` sends the message named by its first "
             "argument, e.g., `a.perform(\\foo)` is `a.foo`. "
             "Use a symbol, e.g., `\\foo` or `'foo'`." };
}

Diagnostic Diagnostic::perform_selector_not_symbol(TextInfoPtr first_t, sc::lex::SourceCodeRange first_l,
                                                   std::string_view string) {
    // Written as a quoted symbol, so it can contain anything
    // the string can.
    std::string symbol;
    // The quotes, escapes are rare so aren't counted.
    symbol.reserve(string.size() + 2);
    symbol.push_back('\'');
    for (const char c : string) {
        if (c == '\'' || c == '\\')
            symbol.push_back('\\');
        symbol.push_back(c);
    }
    symbol.push_back('\'');

    return { "Perform selector is not a symbol",
             Severity::Error,
             { { std::move(first_t), first_l, "did you mean " + symbol + "?" } },
             "`perform` sends the message named by its first "
             "argument, e.g., `a.perform(\\foo)` is `a.foo`." };
}

Diagnostic Diagnostic::not_constant(TextInfoPtr first_t, sc::lex::SourceCodeRange first_l) {
    return { "Not a constant",
             Severity::Error,
             { { std::move(first_t), first_l, "this can't be evaluated when compiling." } },
             "This must be known when the class library is compiled, e.g., a variable's initial value, or an element "
             "of a literal array `#[...]`. Only literals, constants, and arrays and events of them, can be used." };
}

Diagnostic Diagnostic::variadic_argument_not_last(TextInfoPtr variadic_t, sc::lex::SourceCodeRange variadic_l,
                                                  TextInfoPtr positional_t, sc::lex::SourceCodeRange positional_l) {
    return { "Variadic argument is not last",
             Severity::Error,
             {
                 { std::move(variadic_t), variadic_l, "the elements of this are passed after..." },
                 { std::move(positional_t), positional_l, "...this argument, which is written after it." },
             },
             "A variadic argument, e.g., `*a`, must come after all the positional arguments, e.g., `foo(1, *a)`. "
             "This includes the value of `a[*i] = v`, which is passed last." };
}

Diagnostic Diagnostic::event_key_without_value(TextInfoPtr first_t, sc::lex::SourceCodeRange first_l) {
    return { "Event key without a value",
             Severity::Warning,
             { { std::move(first_t), first_l, "this key has no value." } },
             "The elements of `Event[...]` are pairs of keys and values, e.g., `Event[a: 1, b: 2]`. Add a value, or "
             "remove the key." };
}

Diagnostic Diagnostic::unbounded_range(TextInfoPtr first_t, sc::lex::SourceCodeRange first_l) {
    return { "Range has no end",
             Severity::Error,
             { { std::move(first_t), first_l, "this would create an infinite array." } },
             "A range without an end, e.g., `(1..)`, can only be iterated, e.g., in a generator. Add an end, e.g., "
             "`(1..10)`." };
}

Diagnostic Diagnostic::range_too_many_arguments(TextInfoPtr first_t, sc::lex::SourceCodeRange first_l) {
    return { "Too many arguments in range",
             Severity::Error,
             { { std::move(first_t), first_l, "only the first, and optionally the second, can come before `..`." } },
             "A range is `first..last`, or `first, second..last` where second sets the step, e.g., `x[1, 3..9]`." };
}

Diagnostic Diagnostic::range_argument_not_value(TextInfoPtr first_t, sc::lex::SourceCodeRange first_l) {
    return { "Range argument is not a value",
             Severity::Error,
             { { std::move(first_t), first_l, "keyword and variadic arguments can't be used in a range." } },
             "A range is `first..last`, or `first, second..last` where second sets the step, e.g., `x[1, 3..9]`." };
}

Diagnostic Diagnostic::duplicate_variable(TextInfoPtr first_t, sc::lex::SourceCodeRange first_l, TextInfoPtr second_t,
                                          sc::lex::SourceCodeRange second_l) {
    return { "Variable already declared",
             Severity::Error,
             {
                 { std::move(first_t), first_l, "First declared here..." },
                 { std::move(second_t), second_l, "...declared again here." },
             },
             "Arguments and variables in the same function or method must have different names. Rename one." };
}

Diagnostic Diagnostic::declared_environment_variable(TextInfoPtr t, sc::lex::SourceCodeRange l) {
    return { "Environment variable declared",
             Severity::Error,
             { { std::move(t), l, "this can't be declared." } },
             "Only local variables can be declared with 'var'. Remove the '~' to declare a variable, or assign the "
             "environment variable after the declaration." };
}
}
