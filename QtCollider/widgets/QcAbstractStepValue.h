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

#pragma once

class QcAbstractStepValue {
protected:
    QcAbstractStepValue(): _shiftScale(100.f), _ctrlScale(10.f), _altScale(0.1f) {}
    bool modifyStep(double* step);
    void setShiftScale(double scale) { _shiftScale = scale; }
    void setCtrlScale(double scale) { _ctrlScale = scale; }
    void setAltScale(double scale) { _altScale = scale; }
    double shiftScale() { return _shiftScale; }
    double ctrlScale() { return _ctrlScale; }
    double altScale() { return _altScale; }

private:
    double _shiftScale;
    double _ctrlScale;
    double _altScale;
};
