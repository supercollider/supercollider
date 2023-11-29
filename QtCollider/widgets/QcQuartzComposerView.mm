/************************************************************************
 *
 * Copyright 2013 Scott Wilson
 *
 * This file is part of SuperCollider Qt GUI. Copyright 2010 Jakob Leben (jakob.leben@gmail.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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

#include "QcQuartzComposerView.h"
#include "../QcWidgetFactory.h"
#include <QMouseEvent>

QC_DECLARE_QWIDGET_FACTORY(QcQuartzComposerView);

QcQuartzComposerContainerView::QcQuartzComposerContainerView(QWidget* parent): QMacCocoaViewContainer(0, parent) {
    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
    QCView* qcView = [[QCView alloc] init];

    [qcView setAutostartsRendering:YES];
    [qcView setEventForwardingMask:NSAnyEventMask];

    setCocoaView(qcView);

    [qcView release];
    [pool release];
}

QcQuartzComposerContainerView::~QcQuartzComposerContainerView() {}

// QMacCocoaViewContainer *must* be wrapped in a parent view
// so we make a wrapper here which does most of the work
QcQuartzComposerView::QcQuartzComposerView(QWidget* parent): QWidget(parent) {
    containerView = new QcQuartzComposerContainerView(this);
    mQCView = [(QCView*)containerView->cocoaView() retain];
}

QcQuartzComposerView::~QcQuartzComposerView() { [mQCView release]; }

QSize QcQuartzComposerView::sizeHint() const { return containerView->sizeHint(); }

void QcQuartzComposerView::resizeEvent(QResizeEvent* e) {
    containerView->resize(e->size());
    QWidget::resizeEvent(e);
}

void QcQuartzComposerView::startRendering() { [mQCView startRendering]; }

void QcQuartzComposerView::stopRendering() { [mQCView stopRendering]; }

void QcQuartzComposerView::loadCompositionFromFile(const QString& path) {
    QByteArray byteArray = path.toUtf8();
    const char* cString = byteArray.constData();
    NSString* nsString = [[NSString alloc] initWithUTF8String:cString];
    [mQCView loadCompositionFromFile:nsString];
    [nsString release];
}

void QcQuartzComposerView::setMaxRenderingFrameRate(double rate) { [mQCView setMaxRenderingFrameRate:rate]; }

QVariantList QcQuartzComposerView::inputKeys() const {
    NSArray* inputKeys = [mQCView inputKeys];
    int size = [inputKeys count];
    QVariantList keylist;
    for (int i = 0; i < size; ++i) {
        NSString* name = [inputKeys objectAtIndex:i];
        QString key = QString::fromUtf8([name UTF8String]);
        keylist << QVariant(key);
    }
    return keylist;
}

QVariantList QcQuartzComposerView::outputKeys() const {
    NSArray* outputKeys = [mQCView outputKeys];
    int size = [outputKeys count];
    QVariantList keylist;
    for (int i = 0; i < size; ++i) {
        NSString* name = [outputKeys objectAtIndex:i];
        QString key = QString::fromUtf8([name UTF8String]);
        keylist << QVariant(key);
    }
    return keylist;
}

bool QcQuartzComposerView::hasInputKey(NSString* key) { return [[mQCView inputKeys] containsObject:key]; }

bool QcQuartzComposerView::hasOutputKey(NSString* key) { return [[mQCView outputKeys] containsObject:key]; }

void QcQuartzComposerView::setInputPort(NSString* key, id val) { [mQCView setValue:val forInputKey:key]; }

id QcQuartzComposerView::getInputPort(NSString* key) { return [mQCView valueForInputKey:key]; }

id QcQuartzComposerView::getOutputPort(NSString* key) { return [mQCView valueForOutputKey:key]; }

NSString* QcQuartzComposerView::getTypeForKey(NSString* key) {
    NSDictionary* attributes = [[mQCView attributes] objectForKey:key];
    NSString* type = [attributes objectForKey:QCPortAttributeTypeKey];
    return type;
}
