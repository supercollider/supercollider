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

#pragma once

#include <QVariantList>

#include <QMacCocoaViewContainer>
#import <Quartz/Quartz.h>

class QcQuartzComposerContainerView : public QMacCocoaViewContainer {
    Q_OBJECT

public:
    QcQuartzComposerContainerView(QWidget* parent = 0);
    ~QcQuartzComposerContainerView();
    QSize sizeHint() const { return QSize(150, 150); }
    QSize minimumSizeHint() const { return QSize(30, 30); }
};

class QcQuartzComposerView : public QWidget {
    Q_OBJECT
public:
    QcQuartzComposerView(QWidget* parent = 0);
    ~QcQuartzComposerView();
    QSize sizeHint() const;
    virtual void resizeEvent(QResizeEvent*);
    QcQuartzComposerContainerView* containerView;

    QVariantList inputKeys() const;
    QVariantList outputKeys() const;
    bool hasInputKey(NSString* key);
    bool hasOutputKey(NSString* key);
    void setInputPort(NSString* key, id val);
    id getInputPort(NSString* key);
    id getOutputPort(NSString* key);
    NSString* getTypeForKey(NSString* key);

    Q_INVOKABLE void startRendering();
    Q_INVOKABLE void stopRendering();
    Q_INVOKABLE void loadCompositionFromFile(const QString& path);
    Q_INVOKABLE void setMaxRenderingFrameRate(double rate);

    Q_PROPERTY(QVariantList inputKeys READ inputKeys);
    Q_PROPERTY(QVariantList outputKeys READ outputKeys);

private:
    QVariantList variantForNSObject(id nsObject, NSString* type) const;
    id nsObjectForQVariant(const QVariantList& data);
    QCView* mQCView;
};
