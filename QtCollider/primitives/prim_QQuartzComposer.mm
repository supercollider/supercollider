/************************************************************************
 *
 * Copyright 2013 Scott Wilson (i@scottwilson.ca)
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

#import <Quartz/Quartz.h>
#include "primitives.h"
#include "image_primitive_helper_funcs.h"
#include "../QObjectProxy.h"
#include "../QcObjectFactory.h"
#include "../QcApplication.h"
#include "../Common.h"
#include "../type_codec.hpp"
#include "../metatype.hpp"
#include "../widgets/QcQuartzComposerView.h"

#include <PyrObject.h>
#include <PyrKernel.h>
#include <GC.h>
#include <SCBase.h>

#include <QtMacExtras>

#define QOBJECT_FROM_SLOT(s) ((QObjectProxy*)slotRawPtr(slotRawObject(s)->slots))

extern PyrSymbol *s_proto, *s_parent;
extern int ivxIdentDict_array, ivxIdentDict_size, ivxIdentDict_parent, ivxIdentDict_proto, ivxIdentDict_know;
int identDictPut(struct VMGlobals* g, PyrObject* dict, PyrSlot* key, PyrSlot* value);
extern PyrClass* class_identdict;

namespace QtCollider {

// conversion functions
static int getNSObjectForSCObject(PyrSlot* scobject, id* returnID, VMGlobals* g) {
    int err = errNone;
    id returnObject;
    // find the value type and set appropriately
    if (IsFloat(scobject)) { // it's a float
        float val;
        err = slotFloatVal(scobject, &val);
        if (err) {
            *returnID = NULL;
            return err;
        }
        returnObject = [NSNumber numberWithFloat:val];
    } else if (IsInt(scobject)) { // it's an int
        int val;
        err = slotIntVal(scobject, &val);
        if (err) {
            *returnID = NULL;
            return err;
        }
        returnObject = [NSNumber numberWithInt:val];
    } else if (IsTrue(scobject)) { // it's bool true
        returnObject = [NSNumber numberWithBool:YES];
    } else if (IsFalse(scobject)) { // it's bool false
        returnObject = [NSNumber numberWithBool:NO];
    } else if (isKindOfSlot(scobject, s_string->u.classobj)) { // it's a string
        PyrString* string = slotRawString(scobject);
        if (string->size == 0) {
            *returnID = NULL;
            return errFailed;
        }
        char cstring[string->size + 1];
        slotStrVal(scobject, cstring, string->size + 1);
        returnObject = [NSString stringWithCString:cstring encoding:NSUTF8StringEncoding];
    } else if (isKindOfSlot(scobject, s_color->u.classobj)) { // it's a color
        PyrSlot* slots = slotRawObject(scobject)->slots;

        float red, green, blue, alpha;
        err = slotFloatVal(slots + 0, &red);
        if (!err)
            err = slotFloatVal(slots + 1, &green);
        if (!err)
            err = slotFloatVal(slots + 2, &blue);
        if (!err)
            err = slotFloatVal(slots + 3, &alpha);

        if (err) {
            *returnID = NULL;
            return err;
        }
        returnObject = [NSColor colorWithCalibratedRed:red green:green blue:blue alpha:alpha];
    } else if (isKindOfSlot(scobject, s_identitydictionary->u.classobj)) { // it's a structure (dict)
        PyrObject* array;
        array = slotRawObject(&(slotRawObject(scobject)->slots[ivxIdentDict_array]));
        if (!isKindOf((PyrObject*)array, class_array)) {
            *returnID = NULL;
            return errFailed;
        }
        NSMutableDictionary* structure = [NSMutableDictionary dictionary];
        int len = array->size;

        for (int i = 0; i < len; i = i + 2) {
            PyrSlot* element = array->slots + i;
            if (!IsNil(element)) {
                PyrSymbol* keysymbol;
                err = slotSymbolVal(element, &keysymbol);
                if (err) {
                    *returnID = NULL;
                    return err;
                }
                NSString* key = [NSString stringWithCString:keysymbol->name encoding:NSUTF8StringEncoding];
                id innerSCObject;
                int innerErr = getNSObjectForSCObject(element + 1, &innerSCObject, g);
                if (!innerSCObject) {
                    *returnID = NULL;
                    return innerErr;
                }
                [structure setObject:innerSCObject forKey:key];
            }
        }
        returnObject = structure;
    } else if (isKindOfSlot(scobject, class_array)) { // it's a structure (array)
        PyrSlot* array = scobject;
        int len = slotRawObject(array)->size;
        NSMutableArray* structure = [NSMutableArray arrayWithCapacity:(unsigned)len];

        for (int i = 0; i < len; i++) {
            PyrSlot* element = slotRawObject(array)->slots + i;
            id innerSCObject;
            int innerErr = getNSObjectForSCObject(element, &innerSCObject, g);
            if (!innerSCObject) {
                *returnID = NULL;
                return innerErr;
            }
            [structure addObject:innerSCObject];
        }
        returnObject = structure;
    } else if (isKindOfSlot(scobject, SC_CLASS(Image))) { // it's an image
        SharedImage* img = reinterpret_cast<SharedImage*>(slotRawPtr(slotRawObject(scobject)->slots + 0));
        if ((*img)->isPainting()) {
            *returnID = NULL;
            return errFailed;
        }
        QPixmap pixmap = (*img)->pixmap();
        CGImageRef cgImage = QtMac::toCGImageRef(pixmap);
        returnObject = [[NSImage alloc] initWithCGImage:cgImage size:NSZeroSize];
        CGImageRelease(cgImage);
    } else {
        // it's something else...
        *returnID = NULL;
        return errWrongType;
    }

    if (!returnObject) {
        *returnID = NULL;
        err = errFailed;
    } else {
        *returnID = returnObject;
    }
    return err;
}

static int getSCObjectForNSObject(PyrSlot* slot, id nsObject, NSString* type, VMGlobals* g) {
    if ([type isEqualToString:QCPortTypeBoolean]) {
        SetBool(slot, [nsObject boolValue]);
        return errNone;
    } else if ([type isEqualToString:QCPortTypeIndex]) {
        SetInt(slot, [nsObject intValue]);
        return errNone;
    } else if ([type isEqualToString:QCPortTypeNumber]) {
        SetFloat(slot, [nsObject floatValue]);
        return errNone;
    } else if ([type isEqualToString:QCPortTypeString]) {
        const char* cstr = [nsObject UTF8String];
        PyrString* string = newPyrString(g->gc, cstr, 0, true);
        SetObject(slot, string);
        return errNone;
    } else if ([type isEqualToString:QCPortTypeColor]) {
        PyrObject* colorObj = instantiateObject(g->gc, s_color->u.classobj, 0, false, true);

        PyrSlot* slots = colorObj->slots;
        SetFloat(slots + 0, [nsObject redComponent]);
        SetFloat(slots + 1, [nsObject greenComponent]);
        SetFloat(slots + 2, [nsObject blueComponent]);
        SetFloat(slots + 3, [nsObject alphaComponent]);
        SetObject(slot, colorObj);

        return errNone;
    } else if ([type isEqualToString:QCPortTypeStructure]) {
        // for the moment QC seems to deal with all internal structures as NSCFDictionary
        // but check here to be safe
        if ([nsObject isKindOfClass:[NSDictionary class]]) {
            PyrObject *dict, *array;

            dict = instantiateObject(g->gc, class_identdict, 5, true, false);
            array = newPyrArray(g->gc, 4, 0, false);
            array->size = 4;
            nilSlots(array->slots, array->size);
            SetObject(dict->slots + ivxIdentDict_array, array); // we know that dict is white so don't need GCWrite
            SetObject(slot, dict);

            NSEnumerator* enumerator = [nsObject keyEnumerator];
            id key;

            while ((key = [enumerator nextObject])) {
                id innerNSObject = [nsObject objectForKey:key];
                PyrSlot innerSlot;
                NSString* innerType;

                if ([innerNSObject isKindOfClass:[NSNumber class]]) {
                    if (!strcmp([innerNSObject objCType], @encode(BOOL))) {
                        innerType = QCPortTypeBoolean;
                    } else if (!strcmp([innerNSObject objCType], @encode(int))) {
                        innerType = QCPortTypeIndex;
                    } else
                        innerType = QCPortTypeNumber;
                } else if ([innerNSObject isKindOfClass:[NSColor class]]) {
                    innerType = QCPortTypeColor;
                } else if ([innerNSObject isKindOfClass:[NSString class]]) {
                    innerType = QCPortTypeString;
                } else if ([innerNSObject isKindOfClass:[NSArray class]] ||
                           [innerNSObject isKindOfClass:[NSDictionary class]]) {
                    innerType = QCPortTypeStructure;
                } else
                    return errWrongType; // it's something else

                int err = getSCObjectForNSObject(&innerSlot, innerNSObject, innerType, g);
                if (err)
                    return err;

                PyrSlot outKey;
                // key could be an NSNumber, so convert
                if ([key isKindOfClass:[NSNumber class]]) {
                    key = [key stringValue];
                }
                SetSymbol(&outKey, getsym([key UTF8String]));
                err = identDictPut(g, dict, &outKey, &innerSlot);
                if (err)
                    return err;
            }

            return errNone;
        }

    } else if ([type isEqualToString:QCPortTypeImage]) { // QImage
        NSImage* nsimage = (NSImage*)nsObject;
        CGImageRef cgImage = [nsimage CGImageForProposedRect:NULL context:NULL hints:NULL];
        QPixmap pixmap = QtMac::fromCGImageRef(cgImage);

        PyrObject* imageObj = instantiateObject(g->gc, SC_CLASS(Image), 0, false, true);

        Image* image = new Image;
        image->setPixmap(pixmap);
        initialize_image_object(g, imageObj, image);
        SetObject(slot, imageObj);
        return errNone;
    }

    return errWrongType; // it's something else
}

// primitives
QC_LANG_PRIMITIVE(QQuartzComposer_SetInputPort, 2, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    QObjectProxy* proxy = QOBJECT_FROM_SLOT(r);
    if (!proxy->compareThread())
        return QtCollider::wrongThreadError();
    QcQuartzComposerView* view = static_cast<QcQuartzComposerView*>(proxy->object());

    if (NotSym(a))
        return errWrongType;
    PyrSymbol* keysymbol = slotRawSymbol(a);

    qcSCObjectDebugMsg(1, slotRawObject(r), QStringLiteral("SET: %1").arg(keysymbol->name));

    NSString* key = [NSString stringWithCString:keysymbol->name encoding:NSUTF8StringEncoding];
    if (!(view->hasInputKey(key))) {
        post("There is no port with key \"%s\".\n\n", [key UTF8String]);
        return errFailed;
    }

    id val;
    int err = getNSObjectForSCObject(a + 1, &val, g);
    if (err)
        return err;

    view->setInputPort(key, val);

    return errNone;
}

QC_LANG_PRIMITIVE(QQuartzComposer_GetInputPort, 1, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    QObjectProxy* proxy = QOBJECT_FROM_SLOT(r);
    if (!proxy->compareThread())
        return QtCollider::wrongThreadError();
    QcQuartzComposerView* view = static_cast<QcQuartzComposerView*>(proxy->object());

    if (NotSym(a))
        return errWrongType;
    PyrSymbol* keysymbol = slotRawSymbol(a);

    qcSCObjectDebugMsg(1, slotRawObject(r), QStringLiteral("GET: %1").arg(keysymbol->name));

    NSString* key = [NSString stringWithCString:keysymbol->name encoding:NSUTF8StringEncoding];
    if (!(view->hasInputKey(key))) {
        post("There is no port with key \"%s\".\n\n", [key UTF8String]);
        return errFailed;
    }

    NSString* type = view->getTypeForKey(key);

    id nsObject = view->getInputPort(key);
    int err = getSCObjectForNSObject(r, nsObject, type, g);
    if (err)
        return err;
    return errNone;
}

QC_LANG_PRIMITIVE(QQuartzComposer_GetOutputPort, 1, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    QObjectProxy* proxy = QOBJECT_FROM_SLOT(r);
    if (!proxy->compareThread())
        return QtCollider::wrongThreadError();
    QcQuartzComposerView* view = static_cast<QcQuartzComposerView*>(proxy->object());

    if (NotSym(a))
        return errWrongType;
    PyrSymbol* keysymbol = slotRawSymbol(a);

    qcSCObjectDebugMsg(1, slotRawObject(r), QStringLiteral("GET: %1").arg(keysymbol->name));

    NSString* key = [NSString stringWithCString:keysymbol->name encoding:NSUTF8StringEncoding];
    if (!(view->hasOutputKey(key))) {
        post("There is no port with key \"%s\".\n\n", [key UTF8String]);
        return errFailed;
    }

    NSString* type = view->getTypeForKey(key);

    id nsObject = view->getOutputPort(key);
    int err = getSCObjectForNSObject(r, nsObject, type, g);
    if (err)
        return err;
    return errNone;
}


void defineQcQuartzComposerPrimitives() {
    LangPrimitiveDefiner definer;
    definer.define<QQuartzComposer_SetInputPort>();
    definer.define<QQuartzComposer_GetInputPort>();
    definer.define<QQuartzComposer_GetOutputPort>();
}

} // namespace QtCollider
