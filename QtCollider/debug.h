/************************************************************************
 *
 * Copyright 2010-2011 Jakob Leben (jakob.leben@gmail.com)
 *
 * This file is part of SuperCollider Qt GUI.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ************************************************************************/

namespace QtCollider {
int debugLevel();
void setDebugLevel(int);
}

#include <QString>

extern void postfl(const char* fmt, ...);
extern void error(const char* fmt, ...);

#ifdef QC_DEBUG
#    define qcDebugMsg(LEVEL, MSG)                                                                                     \
        if (LEVEL <= QtCollider::debugLevel()) {                                                                       \
            postfl("Qt: %s\n", QString(MSG).toStdString().c_str());                                                    \
        }
#else
#    define qcDebugMsg(LEVEL, MSG)
#endif

#define qcSCObjectDebugMsg(LEVEL, OBJ, MSG)                                                                            \
    qcDebugMsg(LEVEL, QStringLiteral("[%1] %2").arg(OBJ ? slotRawSymbol(&OBJ->classptr->name)->name : "null").arg(MSG))

#define qcErrorMsg(MSG) error("Qt: %s\n", QString(MSG).toStdString().c_str())

#define qcWarningMsg(MSG) postfl("Qt: %s\n", QString(MSG).toStdString().c_str())
