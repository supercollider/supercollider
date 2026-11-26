/*
    SuperCollider Qt IDE
    Copyright (c) 2026 Jordan Henderson
    http://www.audiosynth.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

#include "code_formatter.hpp"
#include "doc_manager.hpp"
#include "util/standard_dirs.hpp"

#include "sc_format/format.hpp"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>

#include <filesystem>
#include <string>

namespace ScIDE { namespace CodeFormatter {

namespace fs = std::filesystem;
namespace scf = sc::format;

namespace {

fs::path toPath(const QString& path) { return fs::u8path(path.toStdString()); }

QString fromPath(const fs::path& path) { return QString::fromStdString(path.u8string()); }

QString defaultOptionsPath() { return QDir(standardDirectory(ScConfigUserDir)).filePath("sc-format.scd"); }

QString writeOptionsFile(const QString& path, const scf::Style& style) {
    if (!QDir().mkpath(QFileInfo(path).absolutePath()))
        return QCoreApplication::translate("CodeFormatter", "Can't make the directory of %1").arg(path);
    QFile file(path);
    const std::string text = scf::options_file_text(style);
    if (!file.open(QIODevice::WriteOnly) || file.write(text.data(), static_cast<qint64>(text.size())) < 0)
        return QCoreApplication::translate("CodeFormatter", "Can't write %1").arg(path);
    return {};
}

}

QString optionsFileFor(Document* document) {
    if (!document || document->filePath().isEmpty())
        return {};
    const auto found = scf::find_style_file(toPath(document->filePath()));
    return found ? fromPath(*found) : QString();
}

QString defaultOptionsFile(Indentation indentation) {
    const QString path = defaultOptionsPath();
    if (QFileInfo::exists(path))
        return path;
    scf::Style style;
    if (indentation.spaces)
        (void)scf::set_option(style, "indent", static_cast<std::int32_t>(indentation.width));
    else
        (void)scf::set_option(style, "indent", std::string_view { "tab" });
    return writeOptionsFile(path, style).isEmpty() ? path : QString();
}

QString createOptionsFile(const QString& path) { return writeOptionsFile(path, scf::Style {}); }

Result format(Document* document, const QString& text, int firstLine, int lastLine, Indentation indentation,
              bool rewrites) {
    QString options = optionsFileFor(document);
    if (options.isEmpty())
        options = defaultOptionsFile(indentation);
    scf::Style style;
    if (!options.isEmpty()) {
        const auto read = scf::read_style(toPath(options));
        if (const auto* error = std::get_if<std::string>(&read))
            return { Result::Failed, {}, QString::fromStdString(*error), false };
        style = std::get<scf::Style>(read);
    }
    if (!rewrites)
        style = scf::without_rewrites(style);
    const bool keepsIndent = style.indent.kind == scf::Indent::Kind::Keep;
    const bool isClass = document && document->filePath().endsWith(".sc", Qt::CaseInsensitive);
    const scf::LineRange lines { static_cast<std::uint32_t>(firstLine), static_cast<std::uint32_t>(lastLine) };
    const auto result = scf::format(text.toStdString(), isClass ? scf::Kind::Class : scf::Kind::Scd, style, lines);
    if (const auto* error = std::get_if<scf::ParseError>(&result))
        return { Result::Failed, {}, QString::fromStdString(error->message), keepsIndent };
    if (const auto* error = std::get_if<scf::InternalError>(&result))
        return { Result::Failed, {}, QString::fromStdString(error->message), keepsIndent };
    const auto& formatted = std::get<scf::Text>(result);
    for (const auto& unparsed : formatted.unparsed_lines)
        if (unparsed.first <= lines.last && lines.first <= unparsed.last)
            return { Result::Unparsed, {}, {}, keepsIndent };
    return { Result::Formatted, QString::fromStdString(formatted.text), {}, keepsIndent };
}

}}
