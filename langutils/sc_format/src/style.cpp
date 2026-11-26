// Copyright Jordan Henderson 2026
#include "sc_format/format.hpp"

#include "consteval/consteval.hpp"
#include "sc_diagnostic/sc_diagnostic.hpp"

#include <array>
#include <fstream>
#include <iterator>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <system_error>
#include <tuple>
#include <utility>
#include <variant>
#include <vector>

namespace sc::format {

namespace {

////////////////////////////////////////////////////////////////////////////////
// Options
////////////////////////////////////////////////////////////////////////////////

// An option whose value is a name, with the names in the order of the enum's values, `keep` first.
template <typename E, std::size_t N> struct Choice {
    std::string_view key;
    E Style::*field;
    std::array<std::string_view, N> values;
};
template <typename E, std::size_t N>
Choice(std::string_view, E Style::*, std::array<std::string_view, N>) -> Choice<E, N>;

template <typename... V> constexpr std::array<std::string_view, sizeof...(V)> names(V... values) {
    return { values... };
}

// An option whose value is a count, or `keep`.
struct Count {
    std::string_view key;
    std::optional<std::uint8_t> Style::*field;
};

// `indent`: `keep`, `tab`, or a number of spaces.
struct IndentOption {
    std::string_view key;
    Indent Style::*field;
};

// Every option, which set_option(), Style::operator==, and options_help() are made from.
constexpr std::tuple options {
    Choice { "final_newline", &Style::final_newline, names("keep", "one") },
    Choice { "trailing_whitespace", &Style::trailing_whitespace, names("keep", "remove") },
    Count { "max_blank_lines", &Style::max_blank_lines },
    Choice { "unicode_spaces", &Style::unicode_spaces, names("keep", "ascii") },
    IndentOption { "indent", &Style::indent },
    Choice { "indent_method_chains", &Style::indent_method_chains, names("keep", "on", "off") },
    Choice { "indent_regions", &Style::indent_regions, names("keep", "on", "off") },
    Choice { "region_brackets", &Style::region_brackets, names("keep", "own_lines") },
    Choice { "space.around_binary_operators", &Style::space_around_binary_operators, names("keep", "one") },
    Choice { "space.around_keyword_operators", &Style::space_around_keyword_operators, names("keep", "one") },
    Choice { "space.around_assignment", &Style::space_around_assignment, names("keep", "one") },
    Choice { "space.after_comma", &Style::space_after_comma, names("keep", "one") },
    Choice { "space.before_comma", &Style::space_before_comma, names("keep", "none") },
    Choice { "space.before_semicolon", &Style::space_before_semicolon, names("keep", "none") },
    Choice { "space.inside_braces", &Style::space_inside_braces, names("keep", "one", "none") },
    Choice { "space.in_empty_braces", &Style::space_in_empty_braces, names("keep", "one", "none") },
    Choice { "space.before_parameters", &Style::space_before_parameters, names("keep", "one", "none") },
    Choice { "space.after_parameters", &Style::space_after_parameters, names("keep", "one", "none") },
    Choice { "space.inside_parens", &Style::space_inside_parens, names("keep", "none") },
    Choice { "space.inside_brackets", &Style::space_inside_brackets, names("keep", "none") },
    Choice { "space.inside_pipes", &Style::space_inside_pipes, names("keep", "none") },
    Choice { "space.around_parameter_equals", &Style::space_around_parameter_equals, names("keep", "one", "none") },
    Choice { "space.after_key", &Style::space_after_key, names("keep", "one", "none") },
    Choice { "space.around_dot", &Style::space_around_dot, names("keep", "none") },
    Choice { "space.before_control_paren", &Style::space_before_control_paren, names("keep", "one", "none") },
    Choice { "space.before_closure", &Style::space_before_closure, names("keep", "one", "none") },
    Choice { "kr.braces", &Style::kr_braces, names("keep", "on") },
    Choice { "kr.parens", &Style::kr_parens, names("keep", "on") },
    Choice { "kr.brackets", &Style::kr_brackets, names("keep", "on") },
    Choice { "multiline.arrays", &Style::multiline_arrays, names("keep", "one_per_line") },
    Choice { "multiline.arguments", &Style::multiline_arguments, names("keep", "one_per_line") },
    Choice { "multiline.control_pairs", &Style::multiline_control_pairs, names("keep", "one_per_line") },
    Choice { "parameters.pipes", &Style::parameter_pipes, names("keep", "on", "one_line") },
    Choice { "parameters.commas", &Style::parameter_commas, names("keep", "on") },
    Choice { "parameters.default_equals", &Style::parameter_default_equals, names("keep", "on") },
    Choice { "parameters.on_brace_line", &Style::parameters_on_brace_line, names("keep", "on") },
    Choice { "final_semicolon", &Style::final_semicolon, names("keep", "remove") },
    Choice { "region_semicolons", &Style::region_semicolons, names("keep", "insert") },
    Choice { "trailing_closures", &Style::trailing_closures, names("keep", "control", "all") },
    Choice { "control_prefix", &Style::control_prefix, names("keep", "on") },
    Choice { "trailing_comma_lists", &Style::trailing_comma_lists, names("keep", "expand") },
    Choice { "align_columns", &Style::align_columns, names("keep", "on") },
};

template <typename F> void for_each_option(F&& visit) {
    std::apply([&](const auto&... option) { (visit(option), ...); }, options);
}

std::string shown(OptionValue value) {
    if (const auto* name = std::get_if<std::string_view>(&value))
        return "`" + std::string { *name } + "`";
    return "`" + std::to_string(std::get<std::int32_t>(value)) + "`";
}

// Names as a list, e.g., `keep, one, or none`.
template <typename Names> std::string listed(const Names& names) {
    std::string list;
    std::size_t written { 0 };
    for (const auto name : names) {
        const bool is_last = written + 1 == std::size(names);
        if (written > 0 && !is_last)
            list += ", ";
        else if (written > 0)
            list += std::size(names) > 2 ? ", or " : " or ";
        list += name;
        ++written;
    }
    return list;
}

template <typename E, std::size_t N>
std::optional<std::string> set(Style& style, const Choice<E, N>& choice, OptionValue value) {
    if (const auto* given = std::get_if<std::string_view>(&value)) {
        std::uint8_t value_index { 0 };
        for (const auto name : choice.values) {
            if (name == *given) {
                style.*choice.field = static_cast<E>(value_index);
                return std::nullopt;
            }
            ++value_index;
        }
    }
    return std::string { choice.key } + ": " + shown(value) + " isn't " + listed(choice.values);
}

std::optional<std::string> set(Style& style, const Count& count, OptionValue value) {
    if (const auto* name = std::get_if<std::string_view>(&value); name && *name == "keep") {
        (style.*count.field).reset();
        return std::nullopt;
    }
    if (const auto* number = std::get_if<std::int32_t>(&value); number && *number >= 0 && *number <= 255) {
        style.*count.field = static_cast<std::uint8_t>(*number);
        return std::nullopt;
    }
    return std::string { count.key } + ": " + shown(value) + " isn't keep or a number up to 255";
}

std::optional<std::string> set(Style& style, const IndentOption& indent, OptionValue value) {
    auto& field = style.*indent.field;
    if (const auto* name = std::get_if<std::string_view>(&value); name && (*name == "keep" || *name == "tab")) {
        field = { *name == "keep" ? Indent::Kind::Keep : Indent::Kind::Tab, 0 };
        return std::nullopt;
    }
    if (const auto* number = std::get_if<std::int32_t>(&value); number && *number >= 0 && *number <= 255) {
        field = { Indent::Kind::Spaces, static_cast<std::uint8_t>(*number) };
        return std::nullopt;
    }
    return std::string { indent.key } + ": " + shown(value) + " isn't keep, tab, or a number of spaces up to 255";
}

// Whether `key` is an option of the group, e.g., `space.after_comma` of `space`.
bool in_group(std::string_view key, std::string_view group) {
    return key.size() > group.size() && key[group.size()] == '.' && key.substr(0, group.size()) == group;
}

// Whether options are named `<name>.<option>`.
bool group_name(std::string_view name) {
    bool found { false };
    for_each_option([&](const auto& option) { found = found || in_group(option.key, name); });
    return found;
}

// The values besides keep.
template <typename E, std::size_t N> std::string help(const Choice<E, N>& choice) {
    return listed(std::vector<std::string_view>(std::next(choice.values.begin()), choice.values.end()));
}
std::string help(const Count&) { return "a number up to 255"; }
std::string help(const IndentOption&) { return "tab or a number of spaces up to 255"; }

// An option's value as written in an options file, and the values it can take instead, keep apart.
template <typename E, std::size_t N> std::string written(const Style& style, const Choice<E, N>& choice) {
    return "\\" + std::string { choice.values[static_cast<std::size_t>(style.*choice.field)] };
}
std::string written(const Style& style, const Count& count) {
    const auto& count_value = style.*count.field;
    return count_value ? std::to_string(*count_value) : "\\keep";
}
std::string written(const Style& style, const IndentOption& indent) {
    const auto& indent_value = style.*indent.field;
    if (indent_value.kind == Indent::Kind::Tab)
        return "\\tab";
    if (indent_value.kind == Indent::Kind::Spaces)
        return std::to_string(indent_value.spaces);
    return "\\keep";
}
template <typename E, std::size_t N> std::vector<std::string> others(const Choice<E, N>& choice) {
    std::vector<std::string> alternatives;
    for (auto value = std::next(choice.values.begin()); value != choice.values.end(); ++value)
        alternatives.push_back("\\" + std::string { *value });
    return alternatives;
}
std::vector<std::string> others(const Count&) { return { "2" }; }
std::vector<std::string> others(const IndentOption&) { return { "\\tab", "4" }; }

////////////////////////////////////////////////////////////////////////////////
// Options files
////////////////////////////////////////////////////////////////////////////////

// The options file a file with `inherit: true` builds on: the nearest above its directory.
std::optional<std::filesystem::path> parent_style_file(const std::filesystem::path& style_file) {
    std::error_code error;
    const auto absolute_file = std::filesystem::absolute(style_file, error).lexically_normal();
    if (error)
        return std::nullopt;
    // find_style_file() looks from the directory of what it is given, so the directory above this file's.
    const auto directory = absolute_file.parent_path();
    if (directory == directory.parent_path())
        return std::nullopt;
    auto found = find_style_file(directory);
    if (found && found->lexically_normal() == absolute_file)
        return std::nullopt;
    return found;
}

}

bool Style::operator==(const Style& other) const {
    bool same { true };
    for_each_option([&](const auto& option) { same = same && this->*option.field == other.*option.field; });
    return same;
}

std::optional<std::string> set_option(Style& style, std::string_view option, OptionValue value) {
    std::optional<std::string> problem { "unknown option `" + std::string { option } + "`" };
    // A group, e.g., `space`, sets each of its options, if every one takes the value.
    Style grouped { style };
    bool is_group { false };
    for_each_option([&](const auto& candidate) {
        if (candidate.key == option)
            problem = set(style, candidate, value);
        else if (in_group(candidate.key, option)) {
            if (!is_group)
                problem.reset();
            is_group = true;
            if (!problem && set(grouped, candidate, value))
                problem = std::string { option } + ": " + shown(value) + " isn't a value of every "
                    + std::string { option } + " option";
        }
    });
    if (is_group && !problem)
        style = grouped;
    return problem;
}

std::string options_help() {
    std::string help_text;
    for_each_option([&](const auto& option) {
        const std::string key { option.key };
        help_text += "  " + key + std::string(key.size() < 32 ? 32 - key.size() : 1, ' ') + help(option) + "\n";
    });
    return help_text;
}

std::string options_file_text(const Style& style) {
    std::string text =
        "// sc_format options. An option that isn't set is \\keep, which leaves that part of the code as\n"
        "// written. Remove the // before an option to set it. See sc_format's README for each one.\n(\n";
    std::string_view current_group;
    for_each_option([&](const auto& option) {
        const auto group_dot = option.key.find('.');
        const auto option_group =
            group_dot == std::string_view::npos ? std::string_view {} : option.key.substr(0, group_dot);
        if (option_group != current_group) {
            if (!current_group.empty())
                text += "\t),\n";
            if (!option_group.empty())
                text += "\t" + std::string { option_group } + ": (\n";
            current_group = option_group;
        }
        const std::string indent = current_group.empty() ? "\t" : "\t\t";
        const std::string name { current_group.empty() ? option.key : option.key.substr(group_dot + 1) };
        const auto written_value = written(style, option);
        const auto alternatives = others(option);
        if (written_value != "\\keep") {
            text += indent + name + ": " + written_value + ",\n";
            return;
        }
        text += indent + "// " + name + ": " + alternatives.front() + ",";
        const char* separator = " // or ";
        for (auto alternative = std::next(alternatives.begin()); alternative != alternatives.end(); ++alternative) {
            text += separator + *alternative;
            separator = ", ";
        }
        text += "\n";
    });
    if (!current_group.empty())
        text += "\t),\n";
    return text + ")\n";
}

namespace {

constexpr std::string_view options_example = ", e.g., `(final_newline: \\one, max_blank_lines: 2)`";

// Reads the options from an options file's event. Each error starts with `where`, the file.
class OptionsEventReader {
public:
    using InheritOrError = std::variant<bool, std::string>;

    OptionsEventReader(const const_eval::Evaluated& evaluated, std::string where):
        m_evaluated(evaluated),
        m_where(std::move(where)) {}

    // `inherit` first, wherever it is, as the other options go on top of the file it names.
    InheritOrError find_inherit(const ir::ConstExprEvent& event) const {
        bool inherit { false };
        for (const auto& [key_index, value_index] : event) {
            const auto name = name_of(key_index);
            if (!name)
                return m_where + "an option's name isn't a symbol" + std::string { options_example };
            if (*name != "inherit")
                continue;
            const auto* inherit_value = m_evaluated(value_index).get_if<bool>();
            if (!inherit_value)
                return m_where + "inherit: the value isn't true or false";
            inherit = *inherit_value;
        }
        return inherit;
    }

    // Each entry is an option, or a group's event of them, e.g., `space: (after_comma: \one)`. Returns the first
    // problem.
    std::optional<std::string> set_options(Style& style, const ir::ConstExprEvent& event) const {
        for (const auto& [key_index, value_index] : event) {
            const std::string name { *name_of(key_index) };
            if (name == "inherit")
                continue;
            const auto& given = m_evaluated(value_index);
            const auto* group = given.get_if<ir::ConstExprEvent>();
            if (!group) {
                if (auto problem = set_option_from(style, name, given))
                    return problem;
                continue;
            }
            if (!group_name(name))
                return m_where + "unknown option group `" + name + "`";
            for (const auto& [option_key_index, option_value_index] : *group) {
                const auto key = name_of(option_key_index);
                if (!key)
                    return m_where + name + ": an option's name isn't a symbol";
                if (auto problem =
                        set_option_from(style, name + "." + std::string { *key }, m_evaluated(option_value_index)))
                    return problem;
            }
        }
        return std::nullopt;
    }

private:
    std::optional<std::string_view> name_of(ir::ConstExprIndex key) const {
        const auto* symbol = m_evaluated(key).get_if<ir::SymSCSymbol>();
        return symbol ? std::optional { m_evaluated.symbols()(*symbol) } : std::nullopt;
    }

    std::optional<std::string> set_option_from(Style& style, const std::string& name,
                                               const ir::ConstExpression& given) const {
        OptionValue option_value;
        if (const auto* symbol = given.get_if<ir::SymSCSymbol>())
            option_value = m_evaluated.symbols()(*symbol);
        else if (const auto* number = given.get_if<std::int32_t>())
            option_value = *number;
        else
            return m_where + name + ": the value isn't a symbol or an integer";
        if (auto problem = set_option(style, name, option_value))
            return m_where + *problem;
        return std::nullopt;
    }

    const const_eval::Evaluated& m_evaluated;
    std::string m_where;
};

// The options of the file `inherit: true` builds on, the defaults if there is none.
StyleOrError inherited_style(const std::filesystem::path& style_file) {
    if (const auto parent = parent_style_file(style_file))
        return read_style(*parent);
    return Style {};
}

}

StyleOrError parse_style(std::string text, std::optional<std::filesystem::path> path) {
    auto where = path ? path->u8string() + ": " : std::string {};
    const auto evaluated = const_eval::evaluate(std::move(text), path);
    std::string errors;
    for (const auto& diagnostic : evaluated->diagnostics())
        if (diagnostic.severity() == diag::Severity::Error)
            errors += diagnostic.to_string();
    if (!errors.empty())
        return errors;

    const auto* evaluated_value = evaluated->value();
    if (!evaluated_value)
        return where + "the options aren't a constant" + std::string { options_example };
    if (evaluated_value->get_if<ir::Nil>())
        return Style {};
    const auto* event = evaluated_value->get_if<ir::ConstExprEvent>();
    if (!event)
        return where + "the options aren't an event" + std::string { options_example };

    const OptionsEventReader reader { *evaluated, std::move(where) };
    const auto inherit = reader.find_inherit(*event);
    if (const auto* error = std::get_if<std::string>(&inherit))
        return *error;
    auto style = std::get<bool>(inherit) && path ? inherited_style(*path) : StyleOrError { Style {} };
    if (std::holds_alternative<std::string>(style))
        return style;
    if (auto problem = reader.set_options(std::get<Style>(style), *event))
        return *problem;
    return style;
}

StyleOrError read_style(const std::filesystem::path& style_file) {
    std::ifstream file { style_file, std::ios::binary };
    if (!file)
        return style_file.u8string() + ": can't be read";
    std::stringstream contents;
    contents << file.rdbuf();
    return parse_style(contents.str(), style_file);
}

std::optional<std::filesystem::path> find_style_file(const std::filesystem::path& file) {
    std::error_code error;
    auto directory = std::filesystem::absolute(file, error).parent_path();
    while (!directory.empty()) {
        auto candidate = directory / std::filesystem::u8path(style_file_name);
        if (std::filesystem::is_regular_file(candidate, error))
            return candidate;
        if (directory == directory.parent_path())
            break;
        directory = directory.parent_path();
    }
    return std::nullopt;
}

}
