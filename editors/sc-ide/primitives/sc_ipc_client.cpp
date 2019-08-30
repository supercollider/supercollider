/*
 *    SuperCollider Qt IDE
 *    Copyright (c) 2012 Jakob Leben & Tim Blechmann
 *    http://www.audiosynth.com
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 */

#include <QDataStream>
#include <QUuid>
#include <QBuffer>
#include <QMutex>

#include <cstdlib>

#include <yaml-cpp/emitter.h>
#include <yaml-cpp/node/node.h>
#include <yaml-cpp/parser.h>

#include "PyrPrimitive.h"
#include "SCBase.h"
#include "GC.h"
#include "PyrKernel.h"
#include "PyrSymbol.h"

#include "sc_ipc_client.hpp"
#include "localsocket_utils.hpp"

SCIpcClient::SCIpcClient(const char* ideName): mSocket(NULL) {
    mSocket = new QLocalSocket();
    mSocket->connectToServer(QString(ideName));
    connect(mSocket, SIGNAL(readyRead()), this, SLOT(readIDEData()));
}

void SCIpcClient::send(const char* data, size_t length) { mSocket->write(data, length); }

SCIpcClient::~SCIpcClient() { mSocket->disconnectFromServer(); }

void SCIpcClient::readIDEData() {
    mIpcData.append(mSocket->readAll());

    // After we have put the data in the buffer, process it
    int avail = mIpcData.length();
    do {
        if (mReadSize == 0 && avail > 4) {
            mReadSize = ArrayToInt(mIpcData.left(4));
            mIpcData.remove(0, 4);
            avail -= 4;
        }

        if (mReadSize > 0 && avail >= mReadSize) {
            QByteArray baReceived(mIpcData.left(mReadSize));
            mIpcData.remove(0, mReadSize);
            mReadSize = 0;
            avail -= mReadSize;

            QDataStream in(baReceived);
            in.setVersion(QDataStream::Qt_4_6);
            QString selector;
            QVariantList argList;
            in >> selector;
            if (in.status() != QDataStream::Ok)
                return;

            in >> argList;
            if (in.status() != QDataStream::Ok)
                return;

            onResponse(selector, argList);
        }
    } while ((mReadSize == 0 && avail > 4) || (mReadSize > 0 && avail > mReadSize));
}

void SCIpcClient::onResponse(const QString& selector, const QVariantList& argList) {
    static QString upDateDocTextSelector("updateDocText");
    static QString upDateDocSelectionSelector("updateDocSelection");

    if (selector == upDateDocTextSelector)
        updateDocText(argList);
    if (selector == upDateDocSelectionSelector)
        updateDocSelection(argList);
}

void SCIpcClient::updateDocText(const QVariantList& argList) {
    QByteArray quuid = argList[0].toByteArray();
    int pos = argList[1].toInt();
    int charsRemoved = argList[2].toInt();
    QString newChars = argList[3].toString();
#ifdef DEBUG_IPC
    post("RECEIVED updateDocText with args id: %s, pos: %d, charsR: %d, newC: %s\n", quuid.constData(), pos,
         charsRemoved, newChars.toLatin1().data());
#endif
    setTextMirrorForDocument(quuid, newChars, pos, charsRemoved);
}

void SCIpcClient::updateDocSelection(const QVariantList& argList) {
    QByteArray quuid = argList[0].toByteArray();
    int start = argList[1].toInt();
    int range = argList[2].toInt();
#ifdef DEBUG_IPC
    post("RECEIVED updateDocText with args id: %s, pos: %d, charsR: %d, newC: %s\n", quuid.constData(), pos,
         charsRemoved, newChars.toLatin1().data());
#endif
    setSelectionMirrorForDocument(quuid, start, range);
}

QString SCIpcClient::getTextMirrorForDocument(QByteArray& id, int pos, int range) {
    QString returnText;
    if (mDocumentTextMirrors.contains(id)) {
        if ((pos == 0) && range == -1) {
            mTextMirrorHashMutex.lock();
            returnText = mDocumentTextMirrors[id];
            mTextMirrorHashMutex.unlock();
        } else {
            mTextMirrorHashMutex.lock();
            QString existingText = mDocumentTextMirrors[id];
            if (range == -1)
                range = existingText.size() - pos;
            QStringRef returnTextRef = QStringRef(&existingText, pos, range);
            returnText = returnTextRef.toString();
            mTextMirrorHashMutex.unlock();
        }
    } else {
        post("WARNING: Attempted to access missing Text Mirror for Document %s\n", id.constData());
    }
    return returnText;
}

void SCIpcClient::setTextMirrorForDocument(QByteArray& id, const QString& text, int pos, int range) {
    if ((pos == 0) && range == -1) {
        mTextMirrorHashMutex.lock();
        mDocumentTextMirrors[id] = text;
        mTextMirrorHashMutex.unlock();
    } else {
        if (mDocumentTextMirrors.contains(id)) {
            mTextMirrorHashMutex.lock();
            QString existingText = mDocumentTextMirrors[id];
            int size = existingText.size();
            if (pos > size)
                pos = size;
            if (range == -1)
                range = existingText.size() - pos;
            mDocumentTextMirrors[id] = existingText.replace(pos, range, text);
            mTextMirrorHashMutex.unlock();
        } else {
            post("WARNING: Attempted to modify missing Text Mirror for Document %s\n", id.constData());
        }
    }
}

QPair<int, int> SCIpcClient::getSelectionMirrorForDocument(QByteArray& id) {
    QPair<int, int> selection;
    if (mDocumentSelectionMirrors.contains(id)) {
        mSelMirrorHashMutex.lock();
        selection = mDocumentSelectionMirrors[id];
        mSelMirrorHashMutex.unlock();
    } else {
        post("WARNING: Attempted to access missing Selection Mirror for Document %s\n", id.constData());
        selection = qMakePair(0, 0);
    }
    return selection;
}

void SCIpcClient::setSelectionMirrorForDocument(QByteArray& id, int start, int range) {
    mSelMirrorHashMutex.lock();
    mDocumentSelectionMirrors[id] = qMakePair(start, range);
    mSelMirrorHashMutex.unlock();
}


static SCIpcClient* gIpcClient = NULL;


int ScIDE_Connect(struct VMGlobals* g, int numArgsPushed) {
    if (gIpcClient) {
        error("ScIDE already connected\n");
        return errFailed;
    }

    PyrSlot* ideNameSlot = g->sp;

    char ideName[1024];
    int status = slotStrVal(ideNameSlot, ideName, 1024);
    if (status != errNone)
        return errWrongType;

    gIpcClient = new SCIpcClient(ideName);

    return errNone;
}

int ScIDE_Connected(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* returnSlot = g->sp - numArgsPushed + 1;

    SetBool(returnSlot, gIpcClient != 0);

    return errNone;
}

struct YAMLSerializer {
    YAML::Emitter emitter;

public:
    explicit YAMLSerializer(PyrSlot* slot) { serialize(slot); }

    const char* data() { return emitter.c_str(); }

    size_t size() { return emitter.size(); }

private:
    void serialize(PyrSlot* slot) {
        if (IsFloat(slot)) {
            emitter << slotRawFloat(slot);
            return;
        }

        switch (GetTag(slot)) {
        case tagNil:
            emitter << YAML::Null;
            return;

        case tagInt:
            emitter << slotRawInt(slot);
            return;

        case tagFalse:
            emitter << false;
            return;

        case tagTrue:
            emitter << true;
            return;

        case tagObj:
            serialize(slotRawObject(slot));
            return;

        case tagSym:
            emitter << YAML::DoubleQuoted << slotRawSymbol(slot)->name;
            return;

        default:
            printf("type: %d\n", GetTag(slot));
            throw std::runtime_error("YAMLSerializer: not implementation for this type");
        }
    }

    void serialize(PyrObject* object) {
        if (isKindOf(object, class_string)) {
            PyrObjectHdr* hdr = static_cast<PyrObjectHdr*>(object);
            PyrString* str = static_cast<PyrString*>(hdr);

            size_t len = str->size;
            char* cstr = new char[len + 10];
            memcpy(cstr, str->s, len);
            cstr[len] = 0; // zero-terminate
            emitter << YAML::DoubleQuoted << cstr;
            delete[] cstr;
            return;
        }

        if (isKindOf(object, class_arrayed_collection)) {
            emitter << YAML::BeginSeq;
            for (size_t i = 0; i != object->size; ++i)
                serialize(object->slots + i);

            emitter << YAML::EndSeq;
            return;
        }
        throw std::runtime_error("YAMLSerializer: not implementation for this type");
    }
};

int ScIDE_Send(struct VMGlobals* g, int numArgsPushed) {
    if (!gIpcClient) {
        error("ScIDE not connected\n");
        return errFailed;
    }

    if (!gMainVMGlobals->canCallOS) {
        error(
            "You can not use ScIDE:prSend functionality in the current thread.\nTry scheduling on AppClock instead.\n");
        return errFailed;
    }

    PyrSlot* idSlot = g->sp - 1;
    char id[255];
    if (slotStrVal(idSlot, id, 255))
        return errWrongType;

    PyrSlot* argSlot = g->sp;

    try {
        YAMLSerializer serializer(argSlot);
        sendSelectorAndData(gIpcClient->mSocket, QString(id), QString::fromUtf8(serializer.data()));
    } catch (std::exception const& e) {
        postfl("Exception during ScIDE_Send: %s\n", e.what());
        return errFailed;
    }

    return errNone;
}

int ScIDE_GetQUuid(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* returnSlot = g->sp - numArgsPushed + 1;

    SetSymbol(returnSlot, getsym(QUuid::createUuid().toString().toLatin1().constData()));

    return errNone;
}

int ScIDE_GetDocTextMirror(struct VMGlobals* g, int numArgsPushed) {
    if (!gIpcClient) {
        error("ScIDE not connected\n");
        return errFailed;
    }

    PyrSlot* returnSlot = g->sp - numArgsPushed + 1;

    PyrSlot* docIDSlot = g->sp - 2;
    char id[255];
    if (slotStrVal(docIDSlot, id, 255))
        return errWrongType;

    int pos, range, err = errNone;
    PyrSlot* posSlot = g->sp - 1;
    err = slotIntVal(posSlot, &pos);
    if (err)
        return err;

    PyrSlot* rangeSlot = g->sp;
    err = slotIntVal(rangeSlot, &range);
    if (err)
        return err;

    QByteArray key = QByteArray(id);

    QString docText = gIpcClient->getTextMirrorForDocument(key, pos, range);

    PyrString* pyrString = newPyrString(g->gc, docText.toLatin1().constData(), 0, true);
    SetObject(returnSlot, pyrString);

    return errNone;
}

int ScIDE_SetDocTextMirror(struct VMGlobals* g, int numArgsPushed) {
    if (!gIpcClient) {
        error("ScIDE not connected\n");
        return errFailed;
    }

    PyrSlot* docIDSlot = g->sp - 3;
    char id[255];
    if (slotStrVal(docIDSlot, id, 255))
        return errWrongType;

    PyrSlot* textSlot = g->sp - 2;

    int length = slotStrLen(textSlot);

    if (length == -1)
        return errWrongType;

    std::vector<char> text(length + 1);

    if (slotStrVal(textSlot, text.data(), length + 1))
        return errWrongType;

    int pos, range, err = errNone;
    PyrSlot* posSlot = g->sp - 1;
    err = slotIntVal(posSlot, &pos);
    if (err)
        return err;

    PyrSlot* rangeSlot = g->sp;
    err = slotIntVal(rangeSlot, &range);
    if (err)
        return err;

    QByteArray key = QByteArray(id);
    QString docText = QString(text.data());

    gIpcClient->setTextMirrorForDocument(key, docText, pos, range);

    return errNone;
}

int ScIDE_GetDocSelectionStart(struct VMGlobals* g, int numArgsPushed) {
    if (!gIpcClient) {
        error("ScIDE not connected\n");
        return errFailed;
    }

    PyrSlot* returnSlot = g->sp - numArgsPushed + 1;

    PyrSlot* docIDSlot = g->sp;
    char id[255];
    if (slotStrVal(docIDSlot, id, 255))
        return errWrongType;

    QByteArray key = QByteArray(id);

    QPair<int, int> selection = gIpcClient->getSelectionMirrorForDocument(key);

    SetInt(returnSlot, selection.first);

    return errNone;
}

int ScIDE_GetDocSelectionRange(struct VMGlobals* g, int numArgsPushed) {
    if (!gIpcClient) {
        error("ScIDE not connected\n");
        return errFailed;
    }

    PyrSlot* returnSlot = g->sp - numArgsPushed + 1;

    PyrSlot* docIDSlot = g->sp;
    char id[255];
    if (slotStrVal(docIDSlot, id, 255))
        return errWrongType;

    QByteArray key = QByteArray(id);

    QPair<int, int> selection = gIpcClient->getSelectionMirrorForDocument(key);

    SetInt(returnSlot, selection.second);

    return errNone;
}

int ScIDE_SetDocSelectionMirror(struct VMGlobals* g, int numArgsPushed) {
    if (!gIpcClient) {
        error("ScIDE not connected\n");
        return errFailed;
    }

    PyrSlot* docIDSlot = g->sp - 2;
    char id[255];
    if (slotStrVal(docIDSlot, id, 255))
        return errWrongType;

    int start, range, err = errNone;
    PyrSlot* startSlot = g->sp - 1;
    err = slotIntVal(startSlot, &start);
    if (err)
        return err;

    PyrSlot* rangeSlot = g->sp;
    err = slotIntVal(rangeSlot, &range);
    if (err)
        return err;

    QByteArray key = QByteArray(id);

    gIpcClient->setSelectionMirrorForDocument(key, start, range);

    return errNone;
}

void initScIDEPrimitives() {
    int base = nextPrimitiveIndex();
    int index = 0;
    definePrimitive(base, index++, "_ScIDE_Connect", ScIDE_Connect, 2, 0);
    definePrimitive(base, index++, "_ScIDE_Connected", ScIDE_Connected, 1, 0);
    definePrimitive(base, index++, "_ScIDE_Send", ScIDE_Send, 3, 0);
    definePrimitive(base, index++, "_ScIDE_GetQUuid", ScIDE_GetQUuid, 0, 0);
    definePrimitive(base, index++, "_ScIDE_GetDocTextMirror", ScIDE_GetDocTextMirror, 4, 0);
    definePrimitive(base, index++, "_ScIDE_SetDocTextMirror", ScIDE_SetDocTextMirror, 5, 0);
    definePrimitive(base, index++, "_ScIDE_GetDocSelectionStart", ScIDE_GetDocSelectionStart, 2, 0);
    definePrimitive(base, index++, "_ScIDE_GetDocSelectionRange", ScIDE_GetDocSelectionRange, 2, 0);
    definePrimitive(base, index++, "_ScIDE_SetDocSelectionMirror", ScIDE_SetDocSelectionMirror, 4, 0);
}
