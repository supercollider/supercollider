/*
    SuperCollider Qt IDE
    Copyright (c) 2012-2013 Jakob Leben & Tim Blechmann
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

#define QT_NO_DEBUG_OUTPUT

#include "docklet.hpp"
#include "gui_utilities.hpp"

#include <QWidget>
#include <QToolButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QIcon>
#include <QAction>
#include <QMenu>
#include <QPainter>
#include <QStyle>
#include <QFontMetrics>
#include <QEvent>
#include <QDebug>

namespace ScIDE {

DockletToolButton::DockletToolButton(QWidget* parent): QToolButton(parent) {}


void DockletToolButton::mouseDoubleClickEvent(QMouseEvent* event) { event->accept(); }

void DockletToolButton::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::RightButton)
        event->accept();
    else
        QToolButton::mousePressEvent(event);
}


DockletToolBar::DockletToolBar(const QString& title) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    mOptionsMenu = new QMenu(this);

    QToolButton* optionsBtn = new DockletToolButton;
    optionsBtn->setIcon(optionsBtn->style()->standardIcon(QStyle::SP_TitleBarNormalButton));
    optionsBtn->setIconSize(QSize(16, 16));
    optionsBtn->setMenu(mOptionsMenu);
    optionsBtn->setPopupMode(QToolButton::InstantPopup);
    optionsBtn->setToolButtonStyle(Qt::ToolButtonIconOnly);
    optionsBtn->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    QLabel* titleLabel = new QLabel(title);
    titleLabel->setMargin(5);
    titleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QHBoxLayout* l = new QHBoxLayout();
    l->setContentsMargins(0, 0, 0, 0);
    l->setSpacing(0);
    l->addWidget(optionsBtn);
    l->addWidget(titleLabel);
    setLayout(l);
}

void DockletToolBar::addAction(QAction* action) {
    QToolButton* btn = new DockletToolButton;
    btn->setIconSize(QSize(16, 16));
    btn->setDefaultAction(action);
    if (!btn->icon().isNull())
        btn->setToolButtonStyle(Qt::ToolButtonIconOnly);
    btn->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    if (layout())
        layout()->addWidget(btn);
}

void DockletToolBar::addWidget(QWidget* widget, int stretch) {
    static_cast<QHBoxLayout*>(layout())->addWidget(widget, stretch);
}

void DockletToolBar::paintEvent(QPaintEvent* event) {
    QPainter painter(this);

    QRect r = rect();

    painter.setBrush(palette().color(QPalette::Mid));
    painter.setPen(Qt::NoPen);
    painter.drawRect(r);

    painter.setPen(palette().color(QPalette::Shadow));
    painter.drawLine(r.bottomLeft(), r.bottomRight());

    // painter.setPen( palette().color(QPalette::Mid).lighter(120) );
    // painter.drawLine( r.topLeft(), r.topRight() );
}


static void updateWindowState(QWidget* window, QDockWidget::DockWidgetFeatures features) {
    Qt::WindowFlags flags = window->windowFlags();
    if (features & QDockWidget::DockWidgetClosable)
        flags |= Qt::WindowCloseButtonHint;
    else
        flags &= ~Qt::WindowCloseButtonHint;
    window->setWindowFlags(flags);
}

Docklet::Docklet(const QString& title, QWidget* parent): QObject(parent), mWindow(0), mWidget(0) {
    mDockWidget = new QDockWidget(title, parent);
    mDockWidget->installEventFilter(this);

    mToolBar = new DockletToolBar(title);
    mDockWidget->setTitleBarWidget(mToolBar);

    QMenu* optionsMenu = mToolBar->optionsMenu();
    QAction* action;

    QDockWidget::DockWidgetFeatures features = mDockWidget->features();

    mDockAction = action = optionsMenu->addAction(tr("Undock"));
    action->setEnabled(features & QDockWidget::DockWidgetFloatable);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(toggleFloating()));

    mDetachAction = action = optionsMenu->addAction(tr("Detach"));
    action->setEnabled(features & QDockWidget::DockWidgetFloatable);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(toggleDetached()));

    action = optionsMenu->addAction(tr("Close"));
    action->setEnabled(features & QDockWidget::DockWidgetClosable);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(close()));

    mVisibilityAction = action = new QAction(title, this);
    action->setCheckable(true);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(setVisible(bool)));

    connect(mDockWidget, SIGNAL(topLevelChanged(bool)), this, SLOT(updateDockAction()));
    connect(mDockWidget, SIGNAL(featuresChanged(QDockWidget::DockWidgetFeatures)), this,
            SLOT(onFeaturesChanged(QDockWidget::DockWidgetFeatures)));
}

void Docklet::toggleFloating() {
    bool undock = !(isDetached() || mDockWidget->isFloating());

    // WARNING: QDockWidget is broken: it internally caches undocked geometry,
    // but only when resized, not when moved.
    // Thus QDockWidget::setFloating may potentially restore wrong geometry,
    // in turn corrupting also our own mUndockedGeom via event filtering!
    // So take measures to remedy that:

    QRect undockedGeom = mUndockedGeom;

    QWidget* container = currentContainer();
    if (container != mDockWidget) {
        container->hide();
        setCurrentContainer(DockableContainer);
    }

    mDockWidget->setFloating(undock);
    mDockWidget->show();

    if (undock) {
        qDebug() << "dock: set geom (toggleFloating):" << undockedGeom << this;
        if (undockedGeom.isNull()) {
            // Looks like resize or move event does not always occur,
            // so store the undocked geometry here.
            mUndockedGeom = mDockWidget->geometry();
        } else {
            mDockWidget->setGeometry(undockedGeom);
        }
    }

    updateDockAction();
}

void Docklet::toggleDetached() { setDetachedAndVisible(!isDetached(), this->isVisible()); }

void Docklet::setDetachedAndVisible(bool detach, bool visible) {
    if (isDetached() == detach)
        return;

    Q_ASSERT(!isDetached() || mWindow != NULL);

    QRect undockedGeom = mUndockedGeom;

    currentContainer()->hide();

    setCurrentContainer(detach ? WindowContainer : DockableContainer);

    if (!detach)
        mDockWidget->setFloating(true);

    QWidget* container = currentContainer();

    // NOTE: Only call show() if the docklet is set to visible otherwise we might
    // get into some timing issue where show() can be called after setVisible(false)
    // https://github.com/supercollider/supercollider/issues/3287
    if (visible)
        container->show();

    // NOTE: set geometry after show() or else some geometry modifying events
    // are postponed!
    qDebug() << (detach ? "win:" : "dock:") << "set geom (setDetachedAndVisible):" << undockedGeom << this;
    if (!undockedGeom.isNull())
        container->setGeometry(undockedGeom);

    updateDockAction();
}

void Docklet::setCurrentContainer(ContainerType containerType) {
    switch (containerType) {
    case DockableContainer:
        mDockWidget->setTitleBarWidget(mToolBar);
        mDockWidget->setWidget(mWidget);
        break;
    case WindowContainer:
        mDockWidget->setWidget(0);
        mDockWidget->setTitleBarWidget(0);

        QVBoxLayout* layout;

        if (!mWindow) {
            mWindow = new QWidget();
            layout = new QVBoxLayout();
            layout->setContentsMargins(0, 0, 0, 0);
            layout->setSpacing(0);
            mWindow->setLayout(layout);
            mWindow->setWindowTitle(mDockWidget->windowTitle());
            mWindow->installEventFilter(this);
            updateWindowState(mWindow, mDockWidget->features());
        } else
            layout = qobject_cast<QVBoxLayout*>(mWindow->layout());

        layout->addWidget(mToolBar);
        layout->addWidget(mWidget);

        mWidget->show();
        mToolBar->show();

        break;
    }

    mDetachAction->setText(containerType == WindowContainer ? tr("Attach") : tr("Detach"));
}

QByteArray Docklet::saveDetachedState() const {
    QByteArray data;

    if (isDetached()) {
        data.append((char)mWindow->isVisible());
        data.append(mWindow->saveGeometry());
    }

    return data;
}

void Docklet::restoreDetachedState(const QByteArray& data) {
    if (!data.isEmpty()) {
        bool visible = data.at(0) == 1;
        setDetachedAndVisible(true, visible);
        mWindow->restoreGeometry(data.mid(1));
    } else
        setDetachedAndVisible(false, this->isVisible());
}

void Docklet::updateDockAction() {
    bool docked = currentContainer() == mDockWidget && !mDockWidget->isFloating();
    mDockAction->setText(docked ? tr("Undock") : tr("Dock"));
}

void Docklet::onFeaturesChanged(QDockWidget::DockWidgetFeatures features) {
    if (mWindow)
        updateWindowState(mWindow, features);
}

bool Docklet::eventFilter(QObject* object, QEvent* event) {
    switch (event->type()) {
    case QEvent::Show:
        // qDebug() << "shown:" << object;
        mVisibilityAction->setChecked(true);
#ifdef Q_OS_MAC
        if (object == mDockWidget && mDockWidget->isFloating()) {
            // qDebug("fitting geometry");
            mDockWidget->setGeometry(fittedToScreen(mDockWidget->geometry(), mDockWidget));
        }
#endif
        break;
    case QEvent::Hide:
        // qDebug() << "hidden:" << object;
        mVisibilityAction->setChecked(false);
        break;
    case QEvent::Resize:
    case QEvent::Move: {
        if (object == currentContainer()) {
            if (object == mWindow) {
                mUndockedGeom = mWindow->geometry();
                qDebug() << "cache window geom" << mUndockedGeom << this;
            } else if (object == mDockWidget && mDockWidget->isFloating()) {
                mUndockedGeom = mDockWidget->geometry();
                qDebug() << "cache dock geom" << mUndockedGeom << this;
            }
        }
        break;
    }
    default:
        break;
    }

    return QObject::eventFilter(object, event);
}

} // namespace ScIDE
