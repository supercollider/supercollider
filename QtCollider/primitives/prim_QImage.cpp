/************************************************************************
*
* This file is a contribution to SuperCollider Qt GUI.
* Copyright 2010-2012 Jakob Leben (jakob.leben@gmail.com)
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

#include "primitives.h"
#include "../QcApplication.h"
#include "../Common.h"
#include "../Slot.h"
#include "../painting.h"

#include <PyrKernel.h>
#include <QImage>
#include <QUrl>
#include <QPainter>

#include <QEventLoop>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

#include <cassert>

static QPainter *imgPainter = 0;

#define QIMAGE_FROM_OBJECT( OBJ ) reinterpret_cast<QImage*>( slotRawPtr(OBJ->slots) );

#define INIT_ASSERT \
  assert( obj->classptr == SC_CLASS( QImage ) ); \
  assert( IsNil( obj->slots ) && IsNil( obj->slots+1 ) );

#define INIT_SETUP \
  SetPtr( obj->slots, img ); // dataptr \
  InstallFinalizer( g, obj, 1, QImage_Finalize ); // finalizer


namespace QtCollider {

int QImage_Finalize( struct VMGlobals *g, struct PyrObject *obj )
{
  delete QIMAGE_FROM_OBJECT( obj );
  SetNil( obj->slots+0 );
  return errNone;
}

void QImage_InitPath( struct VMGlobals *g, struct PyrObject *obj,
                  const QString &path )
{
  INIT_ASSERT
  QImage *img = new QImage( path );
  INIT_SETUP
}

void QImage_InitFromData( struct VMGlobals *g, struct PyrObject *obj,
                          const QByteArray &byteArray )
{
}

void QImage_InitEmpty( struct VMGlobals *g, struct PyrObject *obj,
                       int width, int height )
{
  INIT_ASSERT
  QImage *img = new QImage( width, height, QImage::Format_ARGB32 );
  img->fill( QColor(Qt::black).rgba() );
  INIT_SETUP
}

void QImage_InitWindow( struct VMGlobals *g, struct PyrObject *obj,
                        QWidget *widget, const QRect &rect )
{
}

QC_LANG_PRIMITIVE( QImage_NewPath, 1, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  QString path( Slot::toString(a) );
  QImage_InitPath( g, slotRawObject(r), path );
  return errNone;
}

QC_LANG_PRIMITIVE( QImage_NewURL, 1, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  return errNone;
}

QC_LANG_PRIMITIVE( QImage_NewEmpty, 3, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  if( NotInt(a) || NotInt(a+1) ) return errWrongType;
  int width = Slot::toInt(a);
  int height = Slot::toInt(a+1);

  QImage_InitEmpty( g, slotRawObject(r), width, height );

  return errNone;
}

QC_LANG_PRIMITIVE( QImage_NewFromWindow, 2, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  return errNone;
}

QC_LANG_PRIMITIVE( QImage_Free, 0, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  QImage *img = QIMAGE_FROM_OBJECT( slotRawObject(r) );

  delete img;
  img = new QImage(); // null image, all parameters to zero
  SetPtr( slotRawObject(r)->slots, img );

  return errNone;
}

QC_LANG_PRIMITIVE( QImage_Width, 0, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  QImage *img = QIMAGE_FROM_OBJECT( slotRawObject(r) );
  SetInt( r, img->width() );
  return errNone;
}

QC_LANG_PRIMITIVE( QImage_Height, 0, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  QImage *img = QIMAGE_FROM_OBJECT( slotRawObject(r) );
  SetInt( r, img->height() );
  return errNone;
}

QC_LANG_PRIMITIVE( QImage_Write, 3, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  QImage *img = QIMAGE_FROM_OBJECT( slotRawObject(r) );

  QString path( Slot::toString(a) );
  QString format( Slot::toString(a+1) );

  if(NotInt(a+2)) return errWrongType;
  int quality = Slot::toInt(a+2);

  if( img->save(path, format.toUpper().toStdString().c_str(), quality) )
    return errNone;

  return errFailed;
}

QC_LANG_PRIMITIVE( QImage_SetPainter, 0, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  QImage *img = QIMAGE_FROM_OBJECT( slotRawObject(r) );

  if( !QcApplication::compareThread )
    return QtCollider::wrongThreadError();

  if( imgPainter ) { // should not happen
      qcDebugMsg(1, "WARNING: QImage reset painter");
      QtCollider::endPainting();
      delete imgPainter;
  }

  imgPainter = new QPainter(img);

  if( QtCollider::paintingAnnounced() ) {
    qcDebugMsg(1, "WARNING: Custom painting already in progress. Will not paint." );
    return errNone;
  }

  QtCollider::announcePainting();
  QtCollider::beginPainting( imgPainter );

  return errNone;
}

QC_LANG_PRIMITIVE( QImage_UnsetPainter, 0, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  if( !QcApplication::compareThread )
    return QtCollider::wrongThreadError();

  QtCollider::endPainting();
  delete imgPainter;
  imgPainter = 0;

  return errNone;
}

QC_LANG_PRIMITIVE( QImage_GetPixel, 2, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  if( NotInt(a) || NotInt(a+1)) return errWrongType;

  QImage *img = QIMAGE_FROM_OBJECT( slotRawObject(r) );
  int x = Slot::toInt(a);
  int y = Slot::toInt(a+1);

  if( x >= img->width() || y >= img->height() )
    return errIndexOutOfRange;

  QRgb *line = (QRgb *)img->scanLine(y);
  SetInt(r, (int)line[x]);

  return errNone;
}

QC_LANG_PRIMITIVE( QImage_SetPixel, 2, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  if( NotInt(a) || NotInt(a+1)) return errWrongType;

  QImage *img = QIMAGE_FROM_OBJECT( slotRawObject(r) );
  int argb = Slot::toInt(a);
  int x = Slot::toInt(a+1);
  int y = Slot::toInt(a+2);

  if( x >= img->width() || y >= img->height() )
    return errIndexOutOfRange;

  QRgb *line = (QRgb *)img->scanLine(y);
  line[x] = argb;

  return errNone;
}

QC_LANG_PRIMITIVE( QImage_Fill, 1, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  if( NotObj(a) ) return errWrongType;
  if( slotRawObject(a)->classptr != SC_CLASS(Color) ) return errWrongType;

  QRgb color = Slot::toColor(a).rgba();
  QImage *img = QIMAGE_FROM_OBJECT( slotRawObject(r) );
  img->fill(color);

  return errNone;
}

void defineQImagePrimitives()
{
  LangPrimitiveDefiner definer;
  definer.define<QImage_NewPath>();
  definer.define<QImage_NewURL>();
  definer.define<QImage_NewEmpty>();
  definer.define<QImage_NewFromWindow>();
  definer.define<QImage_Free>();
  definer.define<QImage_Width>();
  definer.define<QImage_Height>();
  definer.define<QImage_Write>();
  definer.define<QImage_SetPainter>();
  definer.define<QImage_UnsetPainter>();
  definer.define<QImage_GetPixel>();
  definer.define<QImage_SetPixel>();
  definer.define<QImage_Fill>();
}

} // namespace QtCollider
