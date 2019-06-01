/************************************************************************
 *
 * Copyright 2012 Jakob Leben (jakob.leben@gmail.com)
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

#include "QcObjectFactory.h"
#include "QcWidgetFactory.h"

#include <QLabel>

QC_DECLARE_QWIDGET_FACTORY(QLabel);

static void doLoadFactories() {
    QC_ADD_FACTORY(QcDefaultWidget);
    QC_ADD_FACTORY(QcHLayoutWidget);
    QC_ADD_FACTORY(QcVLayoutWidget);
    QC_ADD_FACTORY(QLabel);
    QC_ADD_FACTORY(QcTextField);
    QC_ADD_FACTORY(QcCheckBox);
    QC_ADD_FACTORY(QcListWidget);
    QC_ADD_FACTORY(QcPopUpMenu);
    QC_ADD_FACTORY(QcButton);
    QC_ADD_FACTORY(QcCustomPainted);
    QC_ADD_FACTORY(QcFileDialog);
    QC_ADD_FACTORY(QcGraph);
    QC_ADD_FACTORY(QcKnob);
    QC_ADD_FACTORY(QcLevelIndicator);
    QC_ADD_FACTORY(QcMultiSlider);
    QC_ADD_FACTORY(QcNumberBox);
    QC_ADD_FACTORY(QcPenPrinter);
    QC_ADD_FACTORY(QcRangeSlider);
    QC_ADD_FACTORY(QcScope);
    QC_ADD_FACTORY(QcScopeShm);
    QC_ADD_FACTORY(QcScrollWidget);
    QC_ADD_FACTORY(QcScrollArea);
    QC_ADD_FACTORY(QcSlider);
    QC_ADD_FACTORY(QcSlider2D);
    QC_ADD_FACTORY(QcWaveform);
    QC_ADD_FACTORY(QcTextEdit);
    QC_ADD_FACTORY(QcTreeWidget);
    QC_ADD_FACTORY(QcMenu);
    QC_ADD_FACTORY(QcToolBar);
    QC_ADD_FACTORY(QcAction);
    QC_ADD_FACTORY(QcWidgetAction);
    QC_ADD_FACTORY(QcCallback);
    QC_ADD_FACTORY(WebView);
    QC_ADD_FACTORY(QcWindow);
    QC_ADD_FACTORY(QcScrollWindow);
    QC_ADD_FACTORY(QcHBoxLayout);
    QC_ADD_FACTORY(QcVBoxLayout);
    QC_ADD_FACTORY(QcGridLayout);
    QC_ADD_FACTORY(QcStackLayout);
    QC_ADD_FACTORY(QtDownload);
#ifdef __APPLE__
    QC_ADD_FACTORY(QcQuartzComposerView);
#endif
}

namespace QtCollider {

void loadFactories() { doLoadFactories(); }

} // namespace QtCollider
