/************************************************************************
 *
 * Copyright 2011-2012 Jakob Leben (jakob.leben@gmail.com)
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

#include "QcMenu.h"
#include "../QcWidgetFactory.h"

#include <PyrKernel.h>

#include <QKeyEvent>

QC_DECLARE_QWIDGET_FACTORY(QcMenu);
QC_DECLARE_QWIDGET_FACTORY(QcToolBar);
QC_DECLARE_QOBJECT_FACTORY(QcAction);
QC_DECLARE_QOBJECT_FACTORY(QcWidgetAction);

QcMenu::QcMenu(): QMenu(NULL) { setAttribute(Qt::WA_DeleteOnClose, false); }

void QcMenu::popup(QPointF pos, QAction* action) { QMenu::popup(QPoint(pos.x(), pos.y()), action); }

void QcMenu::addAction(QAction* action) {
    if (action) {
        QMenu::addAction(action);
    } else {
        QMenu::addSeparator();
    }
}

void QcMenu::insertAction(QAction* beforeAction, QAction* action) {
    if (action) {
        QMenu::insertAction(beforeAction, action);
    } else {
        QMenu::insertSeparator(beforeAction);
    }
}

void QcMenu::removeAction(QAction* action) {
    if (action) {
        QMenu::removeAction(action);
    }
}

void QcToolBar::addAction(QAction* action) {
    if (action) {
        QToolBar::addAction(action);
    } else {
        QToolBar::addSeparator();
    }
}

void QcToolBar::insertAction(QAction* beforeAction, QAction* action) {
    if (action) {
        QToolBar::insertAction(beforeAction, action);
    } else {
        QToolBar::insertSeparator(beforeAction);
    }
}

void QcToolBar::removeAction(QAction* action) {
    if (action) {
        QToolBar::removeAction(action);
    }
}

QcAction::QcAction(): QAction(NULL) {}

QcWidgetAction::QcWidgetAction(): QWidgetAction(NULL) {}

void QcAction::setIcon(const QtCollider::SharedImage& image) {
    if (image) {
        QIcon icon(image->pixmap());
        QAction::setIcon(icon);
    } else {
        QAction::setIcon(QIcon());
    }
}

QcToolBar::QcToolBar(): QToolBar() {}
