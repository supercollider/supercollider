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

#pragma once

#include <QAction>
#include <QDockWidget>
#include <QMouseEvent>
#include <QToolButton>
#include <QWidget>

namespace ScIDE {

class DockletToolButton : public QToolButton {
    Q_OBJECT

public:
    explicit DockletToolButton(QWidget* parent = nullptr);

private:
    // overrides to hide unwanted effects
    void mouseDoubleClickEvent(QMouseEvent* event) override final;
    void mousePressEvent(QMouseEvent* event) override final;
};

class DockletToolBar : public QWidget {
    Q_OBJECT
public:
    DockletToolBar(const QString& title);

    void addAction(QAction* action);
    void addWidget(QWidget* widget, int stretch = 0);
    QMenu* optionsMenu() { return mOptionsMenu; }

protected:
    virtual void paintEvent(QPaintEvent* event);
    QMenu* mOptionsMenu;
};

class Docklet : public QObject {
    Q_OBJECT
public:
    Docklet(const QString& title, QWidget* parent = 0);

    QDockWidget* dockWidget() { return mDockWidget; }
    QWidget* window() { return mWindow; }
    QWidget* widget() { return mWidget; }
    DockletToolBar* toolBar() { return mToolBar; }
    QAction* toggleViewAction() { return mVisibilityAction; }

    bool isDetached() const { return const_cast<Docklet*>(this)->currentContainer() != mDockWidget; }

    void setDetachedAndVisible(bool detached, bool visible);

    bool isVisible() const { return const_cast<Docklet*>(this)->currentContainer()->isVisible(); }

    void setWidget(QWidget* widget) {
        mWidget = widget;
        if (!isDetached())
            mDockWidget->setWidget(widget);
    }

    void setAllowedAreas(Qt::DockWidgetAreas areas) { mDockWidget->setAllowedAreas(areas); }

    void setFeatures(QDockWidget::DockWidgetFeatures features) { mDockWidget->setFeatures(features); }

    void setObjectName(const QString& name) {
        QObject::setObjectName(name);
        mDockWidget->setObjectName(name);
    }

    QByteArray saveDetachedState() const;
    void restoreDetachedState(const QByteArray&);

public slots:
    void toggleFloating();
    void toggleDetached();
    void setVisible(bool visible) {
        QWidget* container = currentContainer();
        container->setVisible(visible);
        if (visible)
            container->raise();
    }
    void show() { setVisible(true); }
    void hide() { setVisible(false); }
    void close() { hide(); }
    void raise() { currentContainer()->raise(); }
    void focus() {
        show();
        raise();
        mWidget->setFocus();
        mWidget->activateWindow();
    }

private slots:
    void onFeaturesChanged(QDockWidget::DockWidgetFeatures features);
    void updateDockAction();

protected:
    virtual bool eventFilter(QObject* object, QEvent* event);

private:
    enum ContainerType { DockableContainer, WindowContainer };

    QWidget* currentContainer() { return mToolBar->parentWidget(); }

    void setCurrentContainer(ContainerType);

    QDockWidget* mDockWidget;
    QWidget* mWindow;
    QWidget* mWidget;
    DockletToolBar* mToolBar;

    QAction* mDockAction;
    QAction* mDetachAction;
    QAction* mVisibilityAction;

    QRect mUndockedGeom;
};

} // namespace ScIDE
