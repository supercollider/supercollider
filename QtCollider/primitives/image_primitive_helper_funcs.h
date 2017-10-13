/************************************************************************
 *
 * This file is part of SuperCollider Qt GUI.
 *
 * Copyright 2013 Jakob Leben (jakob.leben@gmail.com)
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

// these functions are useful for the creation of QImage SC objects

#ifndef SuperCollider_image_primitive_helper_funcs_h
#define SuperCollider_image_primitive_helper_funcs_h

#include <QImage>
#include "../image.h"
#include "../type_codec.hpp"

namespace QC = QtCollider;

namespace QtCollider {
    
    inline QC::Image * to_image( struct PyrObject * obj )
    {
        SharedImage *shared_image_ptr =
        reinterpret_cast<SharedImage*>( slotRawPtr(obj->slots) );
        return shared_image_ptr->data();
    }
    
    inline QC::Image * to_image( PyrSlot * slot )
    {
        SharedImage *shared_image_ptr =
        reinterpret_cast<SharedImage*>( slotRawPtr( slotRawObject(slot)->slots ) );
        return shared_image_ptr->data();
    }
    
    inline QRgb color_to_pixel( const QColor & color )
    {
        int r, g, b, a;
        color.getRgb( &r, &g, &b, &a );
        qreal k = a / 255.f;
        r *= k;
        g *= k;
        b *= k;
        QRgb pixel = (a << 24) | (r << 16) | (g << 8) | b;
        return pixel;
    }
    
    inline QColor pixel_to_color( QRgb pixel )
    {
        int r, g, b, a;
        int mask = 0xFF;
        a = pixel >> 24 & mask;
        r = pixel >> 16 & mask;
        g = pixel >> 8 & mask;
        b = pixel & mask;
        if ( a > 0 ) {
            qreal k = a > 0 ? 255.f / a : 0.f;
            r *= k;
            g *= k;
            b *= k;
            return QColor(r,g,b,a);
        }
        else
            return QColor(0,0,0,0);
    }
    
    inline int finalize_image_object( struct VMGlobals *g, struct PyrObject *obj )
    {
        SharedImage *shared_image_ptr =
        reinterpret_cast<SharedImage*>( slotRawPtr(obj->slots) );
        delete shared_image_ptr;
        SetNil( obj->slots+0 );
        return errNone;
    }
    
    inline void initialize_image_object( struct VMGlobals *g, struct PyrObject *obj, Image *image )
    {
        assert( IsNil( obj->slots ) && IsNil( obj->slots+1 ) );
        SharedImage *shared_image_ptr = new SharedImage(image);
        SetPtr( obj->slots, shared_image_ptr ); // dataptr
        InstallFinalizer( g, obj, 1, finalize_image_object ); // finalizer
    }
    
} // namespace QtCollider

#endif
