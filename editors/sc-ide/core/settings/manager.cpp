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

#include "manager.hpp"
#include "serialization.hpp"

namespace ScIDE { namespace Settings {

Manager::Manager( const QString & filename, QObject * parent ):
    QObject(parent),
    mSettings( new QSettings(filename, serializationFormat(), this) )
{
    initDefaults();
}

void Manager::initDefaults()
{
    // TODO
}

bool Manager::contains ( const QString & key ) const
{
    if ( mSettings->contains(key) )
        return true;
    else
        return mDefaults.contains( resolvedKey(key) );
}

QVariant Manager::value ( const QString & key, const QVariant & dummy ) const
{
    // FIXME: remove dummy arg, and init default settings instead
    if ( mSettings->contains(key) )
        return mSettings->value(key);
    else
        return mDefaults.value(resolvedKey(key));
}

void Manager::setValue ( const QString & key, const QVariant & value )
{
    mSettings->setValue(key, value);
}

}} // namespace ScIDE::Settings
