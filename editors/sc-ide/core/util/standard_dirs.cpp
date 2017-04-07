/*
    SuperCollider Qt IDE
    Copyright (c) 2013 Jakob Leben & Tim Blechmann
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

#include "standard_dirs.hpp"

#include "SC_Filesystem.hpp"

namespace ScIDE {

QString standardDirectory( StandardDirectory type )
{
    SC_DirectoryName dn;

    switch(type)
    {
        case ScResourceDir:
            dn = SC_DirectoryName::Resource;
            break;

        case ScAppDataSystemDir:
            dn = SC_DirectoryName::SystemAppSupport;
            break;

        case ScAppDataUserDir:
            dn = SC_DirectoryName::UserAppSupport;
            break;

        case ScExtensionSystemDir:
            dn = SC_DirectoryName::SystemExtension;
            break;

        case ScExtensionUserDir:
            dn = SC_DirectoryName::UserExtension;
            break;

        case ScConfigUserDir:
            dn = SC_DirectoryName::UserConfig;
            break;

        default:
            return QString();
    }

    return QString(SC_Filesystem::getDirectory(dn).c_str());
}

} // ScIDE
