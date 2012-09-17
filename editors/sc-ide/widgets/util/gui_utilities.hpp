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

#ifndef SCIDE_WIDGETS_UTIL_GUI_UTILITIES_HPP_INCLUDED
#define SCIDE_WIDGETS_UTIL_GUI_UTILITIES_HPP_INCLUDED

#include <QRegExp>
#include <QDebug>

namespace ScIDE {

inline QString wordInStringAt ( int position, const QString & source )
{
    const QRegExp wordRegexp("\\w+");

    int offset = 0;
    while (offset <= position) {
        int index = wordRegexp.indexIn(source, offset);
        if (index == -1)
            break;
        int len = wordRegexp.matchedLength();
        if (index <= position && index + len >= position)
            return wordRegexp.cap(0);
        offset = index + len;
    }

    return QString();
}

} // namespace ScIDE

#endif // SCIDE_WIDGETS_UTIL_GUI_UTILITIES_HPP_INCLUDED
