/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2012 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: http://www.qt-project.org/
**
**
** GNU Lesser General Public License Usage
**
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this file.
** Please review the following information to ensure the GNU Lesser General
** Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** Other Usage
**
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**************************************************************************/

#include "macfullscreen.h"

#include <AppKit/NSView.h>
#include <AppKit/NSWindow.h>
#include <Foundation/NSNotification.h>

#include <QSysInfo>
#include <qglobal.h>

#if MAC_OS_X_VERSION_MAX_ALLOWED <= MAC_OS_X_VERSION_10_6
enum {
    NSWindowCollectionBehaviorFullScreenPrimary = (1 << 7)
};

static const NSString *NSWindowDidEnterFullScreenNotification = @"NSWindowDidEnterFullScreenNotification";
static const NSString *NSWindowDidExitFullScreenNotification = @"NSWindowDidExitFullScreenNotification";
#endif

@interface WindowObserver : NSObject {
    Core::Internal::MainWindow *window;
}

- (id)initWithMainWindow:(Core::Internal::MainWindow *)w;

- (void)notifyDidEnterFullScreen:(NSNotification *)notification;
- (void)notifyDidExitFullScreen:(NSNotification *)notification;

@end

@implementation WindowObserver

- (id)initWithMainWindow:(Core::Internal::MainWindow *)w;
{
    if ((self = [self init])) {
        window = w;
    }
    return self;
}

- (void)notifyDidEnterFullScreen:(NSNotification *)notification
{
    Q_UNUSED(notification)
    window->setIsFullScreen(true);
}

- (void)notifyDidExitFullScreen:(NSNotification* )notification
{
    Q_UNUSED(notification)
    window->setIsFullScreen(false);
}

@end

static WindowObserver *observer = nil;

using namespace Core::Internal;

bool MacFullScreen::supportsFullScreen()
{
#if QT_VERSION >= 0x040800
    return QSysInfo::MacintoshVersion >= QSysInfo::MV_LION;
#else
    return QSysInfo::MacintoshVersion >= 0x0009; /* MV_LION not defined */
#endif
}

void MacFullScreen::addFullScreen(MainWindow *window)
{
    if (supportsFullScreen()) {
        NSView *nsview = (NSView *) window->winId();
        NSWindow *nswindow = [nsview window];
        [nswindow setCollectionBehavior:NSWindowCollectionBehaviorFullScreenPrimary];

        if (observer == nil)
            observer = [[WindowObserver alloc] initWithMainWindow:window];
        NSNotificationCenter *nc = [NSNotificationCenter defaultCenter];
        [nc addObserver:observer selector:@selector(notifyDidEnterFullScreen:)
            name:NSWindowDidEnterFullScreenNotification object:nswindow];
        [nc addObserver:observer selector:@selector(notifyDidExitFullScreen:)
            name:NSWindowDidExitFullScreenNotification object:nswindow];
    }
}

void MacFullScreen::toggleFullScreen(MainWindow *window)
{
    if (supportsFullScreen()) {
        NSView *nsview = (NSView *) window->winId();
        NSWindow *nswindow = [nsview window];
        [nswindow performSelector:@selector(toggleFullScreen:) withObject: nil];
    }
}
