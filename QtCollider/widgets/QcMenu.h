/************************************************************************
 *
 * Copyright 2011 Jakob Leben (jakob.leben@gmail.com)
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

#ifndef QC_MENU_H
#define QC_MENU_H

#include "../Common.h"
#include "../QObjectProxy.h"
#include "../image.h"
#include "../safeptr.hpp"

#include <VMGlobals.h>
#include <PyrObject.h>

#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QWidgetAction>
#include <QApplication>

using namespace QtCollider;

// @TODO
//  - Split to separate files?
//  - QActionGroup wrapper
//

class QcAction : public QAction {
    friend QObjectProxy;
    Q_OBJECT
public:
    QcAction();

    Q_PROPERTY(const QtCollider::SharedImage& icon READ icon WRITE setIcon);
    Q_PROPERTY(QMenu* menu READ menu WRITE setMenu);
    Q_PROPERTY(QString shortcut READ shortcut WRITE setShortcut);
    Q_PROPERTY(bool separator READ isSeparator WRITE setSeparator);

    QtCollider::SharedImage icon() { return QtCollider::SharedImage(); }
    void setIcon(const QtCollider::SharedImage& image);

    QString shortcut() { return QAction::shortcut().toString(); }
    void setShortcut(QString shortcut) { QAction::setShortcut(QKeySequence(shortcut)); }
};

class QcWidgetAction : public QWidgetAction {
    friend QObjectProxy;
    Q_OBJECT
public:
    QcWidgetAction();

    Q_PROPERTY(QString shortcut READ shortcut WRITE setShortcut);
    Q_PROPERTY(QWidget* defaultWidget READ defaultWidget WRITE setDefaultWidget);

    QString shortcut() { return QAction::shortcut().toString(); }
    void setShortcut(QString shortcut) { QAction::setShortcut(QKeySequence(shortcut)); }

private:
    bool hasCustomWidget;
};


class QcMenu : public QMenu {
    Q_OBJECT

public:
    QcMenu();
    ~QcMenu() {};

    Q_PROPERTY(QList<QAction*> actions READ actions());

    QList<QAction*> actions() const { return QWidget::actions(); }

    Q_INVOKABLE void popup(QPointF pos, QAction* at);

    Q_INVOKABLE void setParent(QWidget* parent) { QMenu::setParent(parent); }

    Q_INVOKABLE void addAction(QAction* action);
    Q_INVOKABLE void insertAction(QAction* before, QAction* action);
    Q_INVOKABLE void removeAction(QAction* action);

    Q_INVOKABLE void clear() { QMenu::clear(); }
};

class QcToolBar : public QToolBar {
    Q_OBJECT

public:
    QcToolBar();

    Q_PROPERTY(QList<QAction*> actions READ actions());

    QList<QAction*> actions() const { return QToolBar::actions(); }

    Q_INVOKABLE void addAction(QAction* action);
    Q_INVOKABLE void insertAction(QAction* before, QAction* action);
    Q_INVOKABLE void removeAction(QAction* action);

    Q_INVOKABLE void clear() { QToolBar::clear(); }
};

Q_DECLARE_METATYPE(QcMenu*);
Q_DECLARE_METATYPE(QcToolBar*);
Q_DECLARE_METATYPE(QcAction*);

#endif
