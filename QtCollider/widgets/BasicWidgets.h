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

#include "QcCanvas.h"
#include "QcMenu.h"
#include "../layouts/classic_layouts.hpp"
#include "image_painter.h"
#include <QBackingStore>

class QcSimpleWidget : public QWidget {
    Q_OBJECT
    Q_PROPERTY(QColor background READ background WRITE setBackground);
    Q_PROPERTY(QList<QAction*> actions READ actions());
    Q_PROPERTY(float devicePixelRatio READ devicePixelRatio);

public:
    const QColor& background() const { return _bkg; }
    void setBackground(const QColor& c);
    Q_INVOKABLE
    void setBackgroundImage(const QtCollider::SharedImage& image, const QRectF& rect, int tileMode, double opacity);
    Q_INVOKABLE
    void removeBackgroundImage() {
        _bkg_image.clear();
        update();
    }

    float devicePixelRatio() const { return backingStore()->window()->devicePixelRatio(); }

    Q_INVOKABLE void addAction(QAction* action) { QWidget::addAction(action); }

    Q_INVOKABLE void removeAction(QAction* action) { QWidget::removeAction(action); }

    Q_INVOKABLE void insertAction(QAction* before, QAction* action) { QWidget::insertAction(before, action); }

protected:
    virtual void paintEvent(QPaintEvent*);

private:
    QColor _bkg;
    QtCollider::ImagePainter _bkg_image;
};

class QcDefaultWidget : public QcSimpleWidget {
    Q_OBJECT
public:
    QcDefaultWidget(): l(this) {}

protected:
    virtual void resizeEvent(QResizeEvent* e) {
        if (!layout())
            l.resize(e);
    }
    QtCollider::DefaultLayout l;
};

class QcHLayoutWidget : public QcSimpleWidget {
    Q_OBJECT
public:
    QcHLayoutWidget(): l(this) {}

protected:
    virtual void resizeEvent(QResizeEvent* e) {
        if (!layout())
            l.resize(e);
    }
    QtCollider::HLayout l;
};

class QcVLayoutWidget : public QcSimpleWidget {
    Q_OBJECT
public:
    QcVLayoutWidget(): l(this) {}

protected:
    virtual void resizeEvent(QResizeEvent* e) {
        if (!layout())
            l.resize(e);
    }
    QtCollider::VLayout l;
};

class QcCustomPainted : public QcCanvas {
    Q_OBJECT
public:
    QcCustomPainted(): l(this) {}

protected:
    // reimplement event handlers just so events don't propagate
    virtual void mousePressEvent(QMouseEvent*) {}
    virtual void mouseReleaseEvent(QMouseEvent*) {}
    virtual void mouseMoveEvent(QMouseEvent*) {}
    virtual void resizeEvent(QResizeEvent* e) {
        QcCanvas::resizeEvent(e);
        if (!layout())
            l.resize(e);
    }
    QtCollider::DefaultLayout l;
};
