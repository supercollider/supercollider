/*
    SuperCollider Qt IDE
    Copyright (c) 2012 Jakob Leben & Tim Blechmann
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

#include "serialization.hpp"

#include <yaml-cpp/yaml.h>

#include <QDebug>
#include <QStringList>
#include <QKeySequence>
#include <boost/iostreams/concepts.hpp>
#include <boost/iostreams/stream.hpp>
#include <iostream>

namespace ScIDE { namespace Settings {

typedef QSettings::SettingsMap::const_iterator SettingsIterator;

struct IODeviceSource : boost::iostreams::source {
    IODeviceSource(QIODevice* dev): mDev(dev) {}
    std::streamsize read(char* s, std::streamsize n) {
        // Read up to n characters from the input
        // sequence into the buffer s, returning
        // the number of characters read, or -1
        // to indicate end-of-sequence.

        qint64 ret = mDev->read(s, n);
        if (ret == 0)
            ret = -1;
        return ret;
    }

    QIODevice* mDev;
};

static QVariant parseTextFormat(const YAML::Node& node) {
    using namespace YAML;

    if (node.Type() != NodeType::Map) {
        qWarning("YAML parsing: a node tagged 'textFormat' has wrong type (not a map)");
        return QVariant();
    }

    std::string val;
    QTextCharFormat fm;

    const Node ncolor = node["color"];
    if (ncolor && ncolor.IsScalar()) {
        val = ncolor.as<std::string>();
        fm.setForeground(QColor(val.c_str()));
    }

    const Node nbg = node["background"];
    if (nbg && nbg.IsScalar()) {
        val = nbg.as<std::string>();
        QColor color(val.c_str());
        if (color.isValid())
            fm.setBackground(color);
    }

    const Node nbold = node["bold"];
    if (nbold && nbold.IsScalar()) {
        bool bold = nbold.as<bool>();
        if (bold)
            fm.setFontWeight(QFont::Bold);
    }

    const Node nitalic = node["italic"];
    if (nitalic && nitalic.IsScalar()) {
        bool italic = nitalic.as<bool>();
        fm.setFontItalic(italic);
    }

    const Node nunder = node["underline"];
    if (nunder && nunder.IsScalar()) {
        bool underline = nunder.as<bool>();
        fm.setFontUnderline(underline);
    }

    return QVariant::fromValue<QTextCharFormat>(fm);
}

static QVariant parseScalar(const YAML::Node& node) {
    using namespace YAML;

    switch (node.Type()) {
    case NodeType::Scalar: {
        std::string val = node.as<std::string>();
        return QVariant(QString::fromUtf8(val.c_str()));
    }

    case NodeType::Sequence: {
        QVariantList list;
        for (auto const& element : node)
            list.append(parseScalar(element));
        return QVariant::fromValue<QVariantList>(list);
    }

    case NodeType::Map: {
        QVariantMap map;
        for (auto const& element : node) {
            std::string key = element.first.as<std::string>();
            QVariant value = parseScalar(element.second);
            map.insert(QString(key.c_str()), value);
        }
        return QVariant::fromValue<QVariantMap>(map);
    }

    case NodeType::Null:
        return QVariant();

    default:
        qWarning("YAML parsing: unsupported node type.");
        return QVariant();
    }
}

static void parseNode(const YAML::Node& node, const QString& parentKey, QSettings::SettingsMap& map) {
    using namespace YAML;

    static const std::string textFormatTag("!textFormat");
    static const std::string qVariantListTag("!QVariantList");
    static const std::string qVariantMapTag("!QVariantMap");

    Q_ASSERT(node.Type() == NodeType::Map);
    for (auto const& element : node) {
        std::string key = element.first.as<std::string>();
        QString childKey(parentKey);
        if (!childKey.isEmpty())
            childKey += "/";
        childKey += key.c_str();

        const YAML::Node& childNode = element.second;
        const std::string& childTag = childNode.Tag();

        if (childTag == textFormatTag)
            map.insert(childKey, parseTextFormat(childNode));
        else if (childTag == qVariantListTag || childTag == qVariantMapTag || childNode.Type() != NodeType::Map)
            map.insert(childKey, parseScalar(childNode));
        else if (childNode.Type() == NodeType::Map)
            parseNode(childNode, childKey, map);
    }
}

bool readSettings(QIODevice& device, QSettings::SettingsMap& map) {
    using namespace YAML;

    try {
        boost::iostreams::stream<IODeviceSource> in(&device);
        Node doc = Load(in);
        if (doc) {
            if (doc.IsMap()) {
                QString key;
                parseNode(doc, key, map);
            }
        }

        return true;
    } catch (std::exception& e) {
        qWarning() << "Exception when parsing YAML config file:" << e.what();
        return false;
    }
}

static void writeTextFormat(const QTextCharFormat& fm, YAML::Emitter& out) {
    out << YAML::LocalTag("textFormat");
    out << YAML::BeginMap;

    if (fm.hasProperty(QTextFormat::ForegroundBrush)) {
        out << YAML::Key << "color";
        out << YAML::Value << fm.foreground().color().name().toStdString();
    }

    if (fm.hasProperty(QTextFormat::BackgroundBrush)) {
        out << YAML::Key << "background";
        out << YAML::Value << fm.background().color().name().toStdString();
    }

    if (fm.hasProperty(QTextFormat::FontWeight)) {
        out << YAML::Key << "bold";
        out << YAML::Value << (fm.fontWeight() == QFont::Bold);
    }

    if (fm.hasProperty(QTextFormat::FontItalic)) {
        out << YAML::Key << "italic";
        out << YAML::Value << fm.fontItalic();
    }

    if (fm.hasProperty(QTextFormat::TextUnderlineStyle)) {
        qDebug("saving underline");
        out << YAML::Key << "underline";
        out << YAML::Value << fm.fontUnderline();
    }

    out << YAML::EndMap;
}

static void writeValue(const QVariant& var, YAML::Emitter& out) {
    switch (var.type()) {
    case QVariant::Invalid: {
        out << YAML::Null;
        break;
    }
    case QVariant::KeySequence: {
        QKeySequence kseq = var.value<QKeySequence>();

        out << kseq.toString(QKeySequence::PortableText).toUtf8().constData();

        break;
    }
    case QVariant::List: {
        out << YAML::LocalTag("QVariantList") << YAML::BeginSeq;

        QVariantList list = var.value<QVariantList>();
        foreach (const QVariant& var, list)
            writeValue(var, out);

        out << YAML::EndSeq;

        break;
    }
    case QVariant::Map: {
        out << YAML::LocalTag("QVariantMap") << YAML::BeginMap;

        QVariantMap map = var.value<QVariantMap>();
        QVariantMap::iterator it;
        for (it = map.begin(); it != map.end(); ++it) {
            out << YAML::Key << it.key().toStdString();
            out << YAML::Value;
            writeValue(it.value(), out);
        }

        out << YAML::EndMap;

        break;
    }
    case QVariant::UserType: {
        int utype = var.userType();

        if (utype == qMetaTypeId<QTextCharFormat>()) {
            writeTextFormat(var.value<QTextCharFormat>(), out);
        } else {
            out << var.toString().toUtf8().constData();
        }
        break;
    }
    default: {
        out << var.toString().toUtf8().constData();
    }
    }
}

static void writeGroup(const QString& groupKey, YAML::Emitter& out, SettingsIterator& it,
                       const QSettings::SettingsMap& map) {
    out << YAML::BeginMap;

    int groupKeyLen = groupKey.size();

    while (it != map.end()) {
        QString key(it.key());

        if (!key.startsWith(groupKey))
            break;

        int i_separ = key.indexOf("/", groupKeyLen);
        if (i_separ != -1) {
            // There is child nodes
            key.truncate(i_separ + 1);

            QString yamlKey(key);
            yamlKey.remove(0, groupKeyLen);
            yamlKey.chop(1);

            out << YAML::Key << yamlKey.toStdString();
            out << YAML::Value;

            writeGroup(key, out, it, map);
        } else {
            // There is no child nodes
            key.remove(0, groupKeyLen);

            out << YAML::Key << key.toStdString();
            out << YAML::Value;

            writeValue(it.value(), out);

            ++it;
        }
    }

    out << YAML::EndMap;
}

bool writeSettings(QIODevice& device, const QSettings::SettingsMap& map) {
    try {
        YAML::Emitter out;
        SettingsIterator it = map.begin();
        writeGroup("", out, it, map);
        device.write(out.c_str());
        return true;
    } catch (std::exception& e) {
        qWarning() << "Exception when writing YAML config file:" << e.what();
        return false;
    }
}

QSettings::Format serializationFormat() {
    static QSettings::Format format = QSettings::registerFormat("yaml", readSettings, writeSettings);

    if (format == QSettings::InvalidFormat)
        qWarning("Could not register settings format");

    return format;
}

void printSettings(const QSettings* settings) {
    using namespace std;

    cout << "config filename: " << settings->fileName().toStdString() << endl;
    QStringList keys = settings->allKeys();
    cout << "num keys: " << keys.count() << endl;
    Q_FOREACH (QString key, keys) {
        QVariant var = settings->value(key);
        if (var.type() == QVariant::Invalid)
            cout << key.toStdString() << ": <null>" << endl;
        else if (var.type() == QVariant::String)
            cout << key.toStdString() << ": " << var.toString().toStdString() << endl;
        else
            cout << key.toStdString() << ": <unknown value type>" << endl;
    }
}

}} // namespace ScIDE::Settings
