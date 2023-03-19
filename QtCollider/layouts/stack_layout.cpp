/************************************************************************
 *
 * Copyright 2012 Jakob Leben (jakob.leben@gmail.com)
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * Contact: Nokia Corporation (qt-info@nokia.com)
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

#include "stack_layout.hpp"

#include <QWidget>

namespace QtCollider {

StackLayout::StackLayout(): _index(-1), _mode(StackOne), _gotParent(false) {}

StackLayout::~StackLayout() { qDeleteAll(_list); }

int StackLayout::addWidget(QWidget* widget) { return insertWidget(_list.count(), widget); }

int StackLayout::insertWidget(int index, QWidget* widget) {
    addChildWidget(widget);
    index = qMin(index, _list.count());
    if (index < 0)
        index = _list.count();
    QWidgetItem* wi = new QWidgetItem(widget);
    _list.insert(index, wi);
    invalidate();
    if (_index < 0) {
        setCurrentIndex(index);
    } else {
        if (index <= _index)
            ++_index;
        if (_mode == StackOne)
            widget->hide();
        widget->lower();
    }
    return index;
}

QLayoutItem* StackLayout::itemAt(int index) const { return _list.value(index); }

QLayoutItem* StackLayout::takeAt(int index) {
    if (index < 0 || index >= _list.size())
        return 0;
    QLayoutItem* item = _list.takeAt(index);
    if (index == _index) {
        _index = -1;
        if (_list.count() > 0) {
            int newIndex = (index == _list.count()) ? index - 1 : index;
            setCurrentIndex(newIndex);
        }
    } else if (index < _index) {
        --_index;
    }
    // NOTE: Here, the Qt implementation hides item->widget() if it exists and is not being
    // deleted. We can't reproduce this, because we can't access private info on whether the
    // widget is being deleted.
    return item;
}

void StackLayout::setCurrentIndex(int index) {
    QWidget* prev = currentWidget();
    QWidget* next = widget(index);
    if (!next || next == prev)
        return;

    _index = index;

    if (!parent())
        return;

    bool reenableUpdates = false;
    QWidget* parent = parentWidget();

    if (parent && parent->updatesEnabled()) {
        reenableUpdates = true;
        parent->setUpdatesEnabled(false);
    }

    QWidget* fw = parent ? parent->window()->focusWidget() : 0;
    if (prev) {
        prev->clearFocus();
        if (_mode == StackOne)
            prev->hide();
    }

    next->raise();
    next->show();

    // try to move focus onto the incoming widget if focus
    // was somewhere on the outgoing widget.

    if (parent) {
        if (fw && (prev && prev->isAncestorOf(fw))) { // focus was on old page
            // look for the best focus widget we can find
            if (QWidget* nfw = next->focusWidget())
                nfw->setFocus();
            else {
                // second best: first child widget in the focus chain
                QWidget* i = fw;
                while ((i = i->nextInFocusChain()) != fw) {
                    if (((i->focusPolicy() & Qt::TabFocus) == Qt::TabFocus) && !i->focusProxy() && i->isVisibleTo(next)
                        && i->isEnabled() && next->isAncestorOf(i)) {
                        i->setFocus();
                        break;
                    }
                }
                // third best: incoming widget
                if (i == fw)
                    next->setFocus();
            }
        }
    }
    if (reenableUpdates)
        parent->setUpdatesEnabled(true);

    if (_mode == StackOne)
        // expandingDirections() might have changed, so invalidate():
        invalidate();
}

int StackLayout::currentIndex() const { return _index; }

void StackLayout::setCurrentWidget(QWidget* widget) {
    int index = indexOf(widget);
    if (index == -1) {
        qWarning("StackLayout::setCurrentWidget: Widget %p not contained in stack", widget);
        return;
    }
    setCurrentIndex(index);
}

QWidget* StackLayout::currentWidget() const { return _index >= 0 ? _list.at(_index)->widget() : 0; }

QWidget* StackLayout::widget(int index) const {
    if (index < 0 || index >= _list.size())
        return 0;
    return _list.at(index)->widget();
}

int StackLayout::count() const { return _list.size(); }

void StackLayout::addItem(QLayoutItem* item) {
    QWidget* widget = item->widget();
    if (widget) {
        addWidget(widget);
        delete item;
    } else {
        qWarning("StackLayout::addItem: Only widgets can be added");
    }
}

QSize StackLayout::sizeHint() const {
    QSize s(0, 0);

    switch (_mode) {
    case StackOne:
        if (_index >= 0)
            if (QWidget* w = _list.at(_index)->widget()) {
                if (w->sizePolicy().horizontalPolicy() != QSizePolicy::Ignored)
                    s.setWidth(w->sizeHint().width());
                if (w->sizePolicy().verticalPolicy() != QSizePolicy::Ignored)
                    s.setHeight(w->sizeHint().height());
            }
        break;

    case StackAll: {
        int n = _list.count();
        for (int i = 0; i < n; ++i)
            if (QWidget* w = _list.at(i)->widget()) {
                QSize ws(w->sizeHint());
                if (w->sizePolicy().horizontalPolicy() == QSizePolicy::Ignored)
                    ws.setWidth(0);
                if (w->sizePolicy().verticalPolicy() == QSizePolicy::Ignored)
                    ws.setHeight(0);
                s = s.expandedTo(ws);
            }
        break;
    }
    }

    return s;
}

static QSize smartMinSize(const QSize& sizeHint, const QSize& minSizeHint, const QSize& minSize, const QSize& maxSize,
                          const QSizePolicy& sizePolicy) {
    QSize s(0, 0);

    if (sizePolicy.horizontalPolicy() != QSizePolicy::Ignored) {
        if (sizePolicy.horizontalPolicy() & QSizePolicy::ShrinkFlag)
            s.setWidth(minSizeHint.width());
        else
            s.setWidth(qMax(sizeHint.width(), minSizeHint.width()));
    }

    if (sizePolicy.verticalPolicy() != QSizePolicy::Ignored) {
        if (sizePolicy.verticalPolicy() & QSizePolicy::ShrinkFlag) {
            s.setHeight(minSizeHint.height());
        } else {
            s.setHeight(qMax(sizeHint.height(), minSizeHint.height()));
        }
    }

    s = s.boundedTo(maxSize);
    if (minSize.width() > 0)
        s.setWidth(minSize.width());
    if (minSize.height() > 0)
        s.setHeight(minSize.height());

    return s.expandedTo(QSize(0, 0));
}

QSize StackLayout::minimumSize() const {
    QSize s(0, 0);

    switch (_mode) {
    case StackOne:
        if (_index >= 0)
            if (QWidget* w = _list.at(_index)->widget())
                s = smartMinSize(w->sizeHint(), w->minimumSizeHint(), w->minimumSize(), w->maximumSize(),
                                 w->sizePolicy());
        break;

    case StackAll: {
        int n = _list.count();
        for (int i = 0; i < n; ++i)
            if (QWidget* w = _list.at(i)->widget())
                s = s.expandedTo(smartMinSize(w->sizeHint(), w->minimumSizeHint(), w->minimumSize(), w->maximumSize(),
                                              w->sizePolicy()));
        break;
    }
    }

    return s;
}

Qt::Orientations StackLayout::expandingDirections() const {
    Qt::Orientations directions;

    switch (_mode) {
    case StackOne:
        directions = _index >= 0 ? _list.at(_index)->expandingDirections() : (Qt::Orientations)0;
        break;

    case StackAll: {
        Qt::Orientations directions = 0;
        int n = _list.count();
        for (int i = 0; i < n; ++i)
            directions |= _list.at(i)->expandingDirections();
        break;
    }
    }

    return directions;
}

void StackLayout::setGeometry(const QRect& rect) {
    switch (_mode) {
    case StackOne:
        if (QWidget* widget = currentWidget())
            widget->setGeometry(rect);
        break;
    case StackAll:
        if (const int n = _list.count())
            for (int i = 0; i < n; ++i)
                if (QWidget* widget = _list.at(i)->widget())
                    widget->setGeometry(rect);
        break;
    }
}

StackLayout::StackingMode StackLayout::stackingMode() const { return _mode; }

void StackLayout::setStackingMode(StackingMode stackingMode) {
    if (_mode == stackingMode)
        return;
    _mode = stackingMode;

    if (!parent())
        return;

    const int n = _list.count();
    if (n == 0)
        return;

    switch (_mode) {
    case StackOne: {
        const int idx = currentIndex();
        if (idx >= 0)
            for (int i = 0; i < n; ++i)
                if (QWidget* widget = _list.at(i)->widget())
                    widget->setVisible(i == idx);
        break;
    }
    case StackAll: { // Turn overlay on: Make sure all widgets are the same size
        QRect geometry;
        if (const QWidget* widget = currentWidget())
            geometry = widget->geometry();
        for (int i = 0; i < n; ++i)
            if (QWidget* widget = _list.at(i)->widget()) {
                if (!geometry.isNull())
                    widget->setGeometry(geometry);
                widget->setVisible(true);
            }
    } break;
    }

    invalidate();
}

void StackLayout::invalidate() {
    QWidget* pw = parentWidget();

    if (pw && !_gotParent) {
        _gotParent = true;

        const int n = _list.count();
        if (n == 0)
            return;

        QWidget* cw = currentWidget();

        switch (_mode) {
        case StackOne: {
            if (cw)
                for (int i = 0; i < n; ++i)
                    if (QWidget* widget = _list.at(i)->widget())
                        widget->setVisible(widget == cw);
            break;
        }
        case StackAll: {
            for (int i = 0; i < n; ++i)
                if (QWidget* widget = _list.at(i)->widget())
                    widget->setVisible(true);
            break;
        }
        }

        // NOTE: re-order the widgets after setting visibility, since the latter
        // may affect the order itself (at least on Mac OS X)
        if (cw) {
            for (int i = 0; i < n; ++i) {
                QWidget* w = _list.at(i)->widget();
                if (w && w != cw)
                    w->lower();
            }
        }
    }

    QLayout::invalidate();
}

} // namespace QtCollider
