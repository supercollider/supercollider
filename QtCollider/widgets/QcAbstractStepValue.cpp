/************************************************************************
 *
 * Copyright 2010 Jakob Leben (jakob.leben@gmail.com)
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

#include <QApplication>

#include "QcAbstractStepValue.h"

bool QcAbstractStepValue::modifyStep(double* pStep) {
    Qt::KeyboardModifiers mods = QApplication::keyboardModifiers();
    if (!mods)
        return false;
    else if (mods & Qt::ShiftModifier)
        *pStep = *pStep * _shiftScale;
    else if (mods & Qt::ControlModifier)
        *pStep = *pStep * _ctrlScale;
    else if (mods & Qt::AltModifier)
        *pStep = *pStep * _altScale;
    else
        return false;

    return true;
}
