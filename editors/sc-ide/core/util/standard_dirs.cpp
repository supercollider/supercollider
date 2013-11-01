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

#include <SC_DirUtils.h>

namespace ScIDE {

QString standardDirectory( StandardDirectory type )
{
    char path[PATH_MAX];

    switch(type)
    {
    case ScResourceDir:
        sc_GetResourceDirectory(path, PATH_MAX);
        break;

    case ScAppDataSystemDir:
        sc_GetSystemAppSupportDirectory(path, PATH_MAX);
        break;

    case ScAppDataUserDir:
        sc_GetUserAppSupportDirectory(path, PATH_MAX);
        break;

    case ScExtensionSystemDir:
        sc_GetSystemExtensionDirectory(path, PATH_MAX);
        break;

    case ScExtensionUserDir:
        sc_GetUserExtensionDirectory(path, PATH_MAX);
        break;

    case ScConfigUserDir:
        sc_GetUserConfigDirectory(path, PATH_MAX);
        break;

    default:
        return QString();
    }

    return QString(path);
}

}
