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

#include "settings.hpp"

#include "yaml-cpp/yaml.h"

#include <QDebug>
#include <QVector>
#include <QStringList>
#include <boost/iostreams/concepts.hpp>
#include <boost/iostreams/stream.hpp>
#include <iostream>

namespace ScIDE {

typedef QSettings::SettingsMap::const_iterator SettingsIterator;

struct IODeviceSource : boost::iostreams::source {

    IODeviceSource ( QIODevice *dev ) : mDev(dev) {}
    std::streamsize read(char* s, std::streamsize n)
    {
        // Read up to n characters from the input
        // sequence into the buffer s, returning
        // the number of characters read, or -1
        // to indicate end-of-sequence.

        qint64 ret = mDev->read(s, n);
        if( ret == 0 ) ret = -1;
        return ret;
    }

    QIODevice *mDev;
};

static void parseSequence( const YAML::Node &node, const QString &parentKey, QSettings::SettingsMap &map )
{
    using namespace YAML;

    QVector<QVariant> values;
    values.reserve(node.size());

    YAML::Iterator it;
    for(it = node.begin(); it != node.end(); ++it) {
        const Node &item = *it;
        switch (item.Type())
        {
            case NodeType::Null:
                values << QVariant();
                break;

            case NodeType::Scalar:
            {
                std::string val;
                item >> val;
                values << QVariant(val.c_str());
                break;
            }

            default:
                qWarning("Configuration file parsing: a sequence item is not a scalar or null." \
                    " This is not supported!");
        }
    }

    map.insert( parentKey, QVariant::fromValue<QVector<QVariant> >(values) );
}

static void parseNode( const YAML::Node &node, const QString &parentKey, QSettings::SettingsMap &map )
{
    using namespace YAML;

    switch (node.Type())
    {
        case NodeType::Null:
            map.insert( parentKey, QVariant() );
            return;

        case NodeType::Scalar:
        {
            std::string val;
            node >> val;
            map.insert( parentKey, QVariant(val.c_str()) );
            return;
        }

        case NodeType::Sequence:
            parseSequence( node, parentKey, map );
            return;

        case NodeType::Map:
        {
            YAML::Iterator it;
            for(it = node.begin(); it != node.end(); ++it) {
                std::string key;
                it.first() >> key;
                QString childKey( parentKey );
                if (!childKey.isEmpty()) childKey += "/";
                childKey += key.c_str();
                parseNode( it.second(), childKey, map );
            }
            return;
        }

    }
}

static void writeValue( const QVariant &var, YAML::Emitter &out )
{
    int type = var.type();

    if(var.type() == QVariant::UserType && var.userType() == qMetaTypeId<QVector<QVariant> >())
    {
        out << YAML::BeginSeq;

        QVector<QVariant> vec = var.value<QVector<QVariant> >();
        Q_FOREACH(QVariant var, vec)
            out << var.toString().toStdString();

        out << YAML::EndSeq;
    }
    else if(var.type() == QVariant::Invalid)
    {
        out << YAML::Null;
    }
    else
    {
        out << var.toString().toStdString();
    }
}

static void writeGroup( const QString &groupKey, YAML::Emitter &out,
                        SettingsIterator &it, const QSettings::SettingsMap &map )
{
    out << YAML::BeginMap;

    int groupKeyLen = groupKey.size();

    while(it != map.end())
    {
        QString key( it.key() );

        if (!key.startsWith(groupKey)) break;

        int i_separ = key.indexOf("/", groupKeyLen);
        if (i_separ != -1) {
            key.truncate( i_separ + 1 );

            QString yamlKey(key);
            yamlKey.remove(0, groupKeyLen);
            yamlKey.chop(1);

            out << YAML::Key << yamlKey.toStdString();
            out << YAML::Value;

            writeGroup( key, out, it, map );
        }
        else
        {
            key.remove(0, groupKeyLen);

            out << YAML::Key << key.toStdString();
            out << YAML::Value;

            writeValue( it.value(), out );

            ++it;
        }
    }

    out << YAML::EndMap;
}

bool readSettings(QIODevice &device, QSettings::SettingsMap &map)
{
    using namespace YAML;

    try {
        boost::iostreams::stream<IODeviceSource> in( &device );
        Parser parser(in);
        Node doc;
        while(parser.GetNextDocument(doc)) {
            if( doc.Type() != NodeType::Map ) continue;
            QString key;
            parseNode( doc, key, map );
        }

        return true;
    }
    catch (std::exception & e) {
        qWarning() << "Exception when parsing YAML config file:" << e.what();
        return false;
    }
}

bool writeSettings(QIODevice &device, const QSettings::SettingsMap &map)
{
    try {
        YAML::Emitter out;
        SettingsIterator it = map.begin();
        writeGroup( "", out, it, map );
        device.write(out.c_str());
        return true;
    }
    catch (std::exception & e) {
        qWarning() << "Exception when writing YAML config file:" << e.what();
        return false;
    }
}

QSettings::Format settingsFormat() {
    static QSettings::Format format =
        QSettings::registerFormat( "yaml", readSettings, writeSettings );

    if( format == QSettings::InvalidFormat )
        qWarning("Could not register settings format");

    return format;
}

void printSettings (const QSettings * settings)
{
    using namespace std;

    cout << "config filename: " << settings->fileName().toStdString() << endl;
    QStringList keys = settings->allKeys();
    cout << "num keys: " << keys.count() << endl;
    Q_FOREACH( QString key, keys ) {
        QVariant var = settings->value(key);
        if (var.type() == QVariant::Invalid)
            cout << key.toStdString() << ": <null>" << endl;
        else if (var.type() == QVariant::String)
            cout << key.toStdString() << ": " << var.toString().toStdString() << endl;
        else if ( var.type() == QVariant::UserType && var.userType() == qMetaTypeId<QVector<QVariant> >() )
        {
            cout << key.toStdString() << ":" << endl;
            QVector<QVariant> vec = var.value<QVector<QVariant> >();
            Q_FOREACH(QVariant var, vec)
                cout << "   - "
                    << (var.type() == QVariant::Invalid ? "<null>" : var.toString().toStdString())
                    << endl;
        }
        else
            cout << key.toStdString() << ": <unknown value type>" << endl;
    }
}

} // namespace ScIDE
