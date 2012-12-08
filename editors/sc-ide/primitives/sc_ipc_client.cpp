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

#include <QLocalSocket>
#include <QDataStream>

#include <cstdlib>

#include <yaml-cpp/emitter.h>

#include "PyrPrimitive.h"
#include "SCBase.h"
#include "GC.h"
#include "PyrKernel.h"
#include "PyrSymbol.h"

class SCIpcClient
{
public:
    QLocalSocket * mSocket;
    SCIpcClient( const char * ideName ):
        mSocket(NULL)
    {
        mSocket = new QLocalSocket();
        mSocket->connectToServer(QString(ideName));
    }

    void send(const char * data, size_t length)
    {
        mSocket->write(data, length);
    }

    ~SCIpcClient()
    {
        mSocket->disconnectFromServer();
    }
};

static SCIpcClient * gIpcClient = NULL;


int ScIDE_Connect(struct VMGlobals *g, int numArgsPushed)
{
    if (gIpcClient) {
        error("ScIDE already connected\n");
        return errFailed;
    }

    PyrSlot * ideNameSlot = g->sp;

    char ideName[1024];
    int status = slotStrVal(ideNameSlot, ideName, 1024);
    if (status != errNone)
        return errWrongType;

    gIpcClient = new SCIpcClient(ideName);

    return errNone;
}

int ScIDE_Connected(struct VMGlobals *g, int numArgsPushed)
{
    PyrSlot * returnSlot = g->sp - numArgsPushed + 1;

    SetBool(returnSlot, gIpcClient != 0);

    return errNone;
}

struct YAMLSerializer
{
    YAML::Emitter emitter;

public:
    explicit YAMLSerializer (PyrSlot * slot)
    {
        serialize(slot);
    }

    const char * data ()
    {
        return emitter.c_str();
    }

    size_t size()
    {
        return emitter.size();
    }

private:
    void serialize(PyrSlot * slot)
    {
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
            printf ("type: %d\n", GetTag(slot));
            throw std::runtime_error("YAMLSerializer: not implementation for this type");
        }
    }

    void serialize(PyrObject * object)
    {
        if (isKindOf(object, class_string)) {
            PyrObjectHdr * hdr = static_cast<PyrObjectHdr*>(object);
            PyrString * str = static_cast<PyrString*>(hdr);

            size_t len = str->size;
            char * cstr = new char[len + 10];
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

int ScIDE_Send(struct VMGlobals *g, int numArgsPushed)
{
    if (!gIpcClient) {
        error("ScIDE not connected\n");
        return errFailed;
    }

    PyrSlot * idSlot = g->sp - 1;
    char id[255];
    if (slotStrVal( idSlot, id, 255 ))
        return errWrongType;

    PyrSlot * argSlot = g->sp;

    try {
        YAMLSerializer serializer(argSlot);

        QDataStream stream(gIpcClient->mSocket);
        stream.setVersion(QDataStream::Qt_4_6);
        stream << QString(id);
        stream << QString::fromUtf8(serializer.data());
    } catch (std::exception const & e) {
        postfl("Exception during ScIDE_Send: %s\n", e.what());
        return errFailed;
    }

    return errNone;
}


void initScIDEPrimitives()
{
    int base = nextPrimitiveIndex();
    int index = 0;
    definePrimitive(base, index++, "_ScIDE_Connect",   ScIDE_Connect, 2, 0);
    definePrimitive(base, index++, "_ScIDE_Connected", ScIDE_Connected, 1, 0);
    definePrimitive(base, index++, "_ScIDE_Send",      ScIDE_Send, 3, 0);
}
