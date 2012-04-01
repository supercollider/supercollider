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

#ifndef SCIDE_CORE_SETTINGS_MANAGER_HPP_INCLUDED
#define SCIDE_CORE_SETTINGS_MANAGER_HPP_INCLUDED

#include "serialization.hpp" // to forward meta type declarations

#include <QObject>

namespace ScIDE { namespace Settings {

class Manager : public QObject
{
    Q_OBJECT

public:
    Manager( const QString & filename, QObject * parent = 0 );

    // forwarded

    void beginGroup ( const QString & prefix ) { mSettings->beginGroup(prefix); }

    void endGroup () { mSettings->endGroup(); }

    QString group() const { return mSettings->group(); }

    QStringList childGroups () const { return mSettings->childGroups(); }

    QStringList childKeys () const { return mSettings->childKeys(); }

    QStringList allKeys() const { return mSettings->allKeys(); }

    void sync() { mSettings->sync(); }

    // extended

    bool contains ( const QString & key ) const;

    QVariant value ( const QString & key, const QVariant & dummy = QVariant() ) const;

    void setValue ( const QString & key, const QVariant & value );

private:
    typedef QSettings::SettingsMap SettingsMap;

    QString resolvedKey( const QString & key ) const
    {
        return mSettings->group().append(key);
    }

    void initDefaults();

    QSettings *mSettings;
    SettingsMap mDefaults;
};

}} // namespace ScIDE::Settings

#endif
