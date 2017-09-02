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

#include "SC_Filesystem.hpp" // getDirectory
#include "SC_Codecvt.hpp" // path_to_utf8_str
#include <boost/filesystem/path.hpp> // path

namespace ScIDE {

QString standardDirectory( StandardDirectory type )
{
    using DirName = SC_Filesystem::DirName;
    DirName dn;

    switch(type)
    {
    case ScResourceDir:
        dn = DirName::Resource;
        break;

    case ScAppDataSystemDir:
        dn = DirName::SystemAppSupport;
        break;

    case ScAppDataUserDir:
        dn = DirName::UserAppSupport;
        break;

    case ScExtensionSystemDir:
        dn = DirName::SystemExtension;
        break;

    case ScExtensionUserDir:
        dn = DirName::UserExtension;
        break;

    case ScConfigUserDir:
        dn = DirName::UserConfig;
        break;

    default:
        return QString();
    }

    const boost::filesystem::path path = SC_Filesystem::instance().getDirectory(dn);
    return QString(SC_Codecvt::path_to_utf8_str(path).c_str());
}

} // ScIDE
