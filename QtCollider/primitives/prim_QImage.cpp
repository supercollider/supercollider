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
#include "../type_codec.hpp"
#include "../painting.h"

#include <PyrKernel.h>
#include <QImage>
#include <QUrl>
#include <QPainter>
#include <QImageReader>
#include <QImageWriter>

#include <QEventLoop>
#include <QTimer>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

#include <cassert>

static QPainter *imgPainter = 0;

#define QIMAGE_FROM_OBJECT( OBJ ) reinterpret_cast<QImage*>( slotRawPtr(OBJ->slots) );

namespace QC = QtCollider;

namespace QtCollider {

int QImage_Finalize( struct VMGlobals *g, struct PyrObject *obj )
{
  delete QIMAGE_FROM_OBJECT( obj );
  SetNil( obj->slots+0 );
  return errNone;
}

inline void QImage_Init( struct VMGlobals *g, struct PyrObject *obj, QImage *img )
{
    assert( IsNil( obj->slots ) && IsNil( obj->slots+1 ) );

    if( img->format() != QImage::Format_ARGB32_Premultiplied ) {
        QImage *aux = img;
        img = new QImage( img->convertToFormat( QImage::Format_ARGB32_Premultiplied ) );
        delete aux;
    }
    SetPtr( obj->slots, img ); // dataptr
    InstallFinalizer( g, obj, 1, QImage_Finalize ); // finalizer
}

bool QImage_InitPath( struct VMGlobals *g, struct PyrObject *obj,
                  const QString &path )
{
  QImage *img = new QImage( path );
  if( img->isNull() ) {
      delete img;
      return false;
  }

  QImage_Init(g, obj, img);

  return true;
}

bool QImage_InitFromData( struct VMGlobals *g, struct PyrObject *obj,
                          const QByteArray &byteArray )
{
  QImage *img = new QImage();
  img->loadFromData( byteArray );
  if( img->isNull() ) {
      delete img;
      return false;
  }

  QImage_Init(g, obj, img);

  return true;
}

void QImage_InitEmpty( struct VMGlobals *g, struct PyrObject *obj,
                       int width, int height )
{
  QImage *img = new QImage( width, height, QImage::Format_ARGB32_Premultiplied );
  img->fill( QColor(Qt::transparent).rgba() );
  QImage_Init(g, obj, img);
}

void QImage_InitWindow( struct VMGlobals *g, struct PyrObject *obj,
                        QWidget *widget, const QRect &rect )
{
  QImage *img = new QImage(QPixmap::grabWidget( widget, rect ).toImage());
  QImage_Init(g, obj, img);
}

QC_LANG_PRIMITIVE( QImage_NewPath, 1, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  QString path( QtCollider::get<QString>(a) );
  if( QImage_InitPath( g, slotRawObject(r), path ) ) {
    return errNone;
  } else {
    qcErrorMsg("QImage file doesn't exist or can't be open");
    return errFailed;
  }
}

QC_LANG_PRIMITIVE( QImage_NewURL, 2, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  QUrl url( QtCollider::get<QString>(a) );
  if( !url.isValid() || url.isEmpty() ) {
    qcErrorMsg("QImage invalid or empty URL");
    return errFailed;
  }

  if( url.isLocalFile() ) {
    if( QImage_InitPath( g, slotRawObject(r), url.toLocalFile() ) ) {
      return errNone;
    } else {
      qcErrorMsg("QImage file doesn't exist or can't be open");
      return errFailed;
    }
  }

  if( !IsFloat(a+1) && !IsInt(a+1) ) return errWrongType;
  // Take a safe read to allow Integers:
  float timeout = QtCollider::get(a+1);

  QNetworkAccessManager *manager = new QNetworkAccessManager();
  QNetworkReply *reply = manager->get( QNetworkRequest( url ) );

  QEventLoop loop;
  QTimer::singleShot( 100 * timeout, &loop, SLOT(quit()) );
  QcApplication::connect( manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()) );
  QcApplication::connect( reply, SIGNAL(error(QNetworkReply::NetworkError)), &loop, SLOT(quit()) );
  loop.exec(); // blocks

  if( reply->error() != QNetworkReply::NoError ) {
    qcErrorMsg(QString("QImage error in download from ").append(url.toString()));
    manager->deleteLater();
    reply->deleteLater();
    return errFailed;
  } else if( !reply->isFinished() ) {
    qcErrorMsg("QImage download timeout");
    reply->abort();
    manager->deleteLater();
    reply->deleteLater();
    return errFailed;
  }

  QByteArray byteArray = reply->readAll();
  if( byteArray.isEmpty() ) {
    qcErrorMsg("QImage information could not be read");
    manager->deleteLater();
    reply->deleteLater();
    return errFailed;
  }

  if( QImage_InitFromData( g, slotRawObject(r), byteArray ) ) {
    manager->deleteLater();
    reply->deleteLater();
    return errNone;
  } else {
    qcErrorMsg("QImage url bynary data is not an image file");
    manager->deleteLater();
    reply->deleteLater();
    return errFailed;
  }
}

QC_LANG_PRIMITIVE( QImage_NewEmpty, 2, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  if( NotInt(a) || NotInt(a+1) ) return errWrongType;
  int width = QtCollider::read<int>(a);
  int height = QtCollider::read<int>(a+1);

  QImage_InitEmpty( g, slotRawObject(r), width, height );

  return errNone;
}

QC_LANG_PRIMITIVE( QImage_NewFromWindow, 2, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  QObjectProxy *proxy = QtCollider::get(a);
  if( !proxy ) return errWrongType;
  QWidget *widget = qobject_cast<QWidget *>( proxy->object() );
  if( !widget ) return errWrongType;

  if( NotObj(a+1) || slotRawObject(a+1)->classptr != SC_CLASS(Rect) ) return errWrongType;
  QRect rect = QtCollider::read<QRect>(a+1);

  QImage_InitWindow( g, slotRawObject(r), widget, rect );

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

QC_LANG_PRIMITIVE( QImage_SetSize, 4, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  if( NotInt(a) || NotInt(a+1) || NotInt(a+2) || NotInt(a+3) ) return errWrongType;
  QSize newSize( QtCollider::read<int>(a), QtCollider::read<int>(a+1) );
  int arMode = QtCollider::read<int>(a+2);
  int trMode = QtCollider::read<int>(a+3);

  QImage *img = QIMAGE_FROM_OBJECT( slotRawObject(r) );
  QImage *res = new QImage(
    img->scaled( newSize, (Qt::AspectRatioMode)arMode, (Qt::TransformationMode)trMode ) );

  SetPtr( slotRawObject(r)->slots, res );
  delete img;

  return errNone;
}

QC_LANG_PRIMITIVE( QImage_Write, 3, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  QImage *img = QIMAGE_FROM_OBJECT( slotRawObject(r) );

  QString path = QC::get(a);
  QString format = QC::get(a+1);

  if(NotInt(a+2)) return errWrongType;
  int quality = QC::read<int>(a+2);

  if( img->save(path, format.toUpper().toStdString().c_str(), quality) )
    return errNone;

  qcErrorMsg( QString("QImage can't write file: ").append(path) );
  return errNone;
}

QC_LANG_PRIMITIVE( QImage_SetPainter, 0, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  if( !QcApplication::compareThread )
    return QtCollider::wrongThreadError();

  if( QtCollider::paintingAnnounced() ) {
    qcDebugMsg(1, "WARNING: Custom painting already in progress. Will not paint." );
    return errNone;
  }

  assert( imgPainter == 0 );

  QImage *img = QIMAGE_FROM_OBJECT( slotRawObject(r) );
  imgPainter = new QPainter(img);

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
  int x = QC::read<int>(a);
  int y = QC::read<int>(a+1);

  if( x >= img->width() || y >= img->height() )
    return errIndexOutOfRange;

  int *line = reinterpret_cast<int*>( img->scanLine(y) );
  SetInt( r, line[x] );

  return errNone;
}

QC_LANG_PRIMITIVE( QImage_GetColor, 2, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  if( NotInt(a) || NotInt(a+1)) return errWrongType;

  QImage *img = QIMAGE_FROM_OBJECT( slotRawObject(r) );
  int x = QC::read<int>(a);
  int y = QC::read<int>(a+1);

  if( x >= img->width() || y >= img->height() )
    return errIndexOutOfRange;

  QRgb *line = reinterpret_cast<QRgb*>( img->scanLine(y) );
  QC::set(r, QColor( line[x] ) );

  return errNone;
}

QC_LANG_PRIMITIVE( QImage_SetPixel, 3, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  if( NotInt(a) || NotInt(a+1) || NotInt(a+2) ) return errWrongType;

  QImage *img = QIMAGE_FROM_OBJECT( slotRawObject(r) );
  int pixel = QC::read<int>(a);
  int x = QC::read<int>(a+1);
  int y = QC::read<int>(a+2);

  if( x >= img->width() || y >= img->height() )
    return errIndexOutOfRange;

  int *line = reinterpret_cast<int*>( img->scanLine(y) );
  line[x] = pixel;

  return errNone;
}

QC_LANG_PRIMITIVE( QImage_SetColor, 3, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  if ( NotObj(a) || slotRawObject(a)->classptr != SC_CLASS(Color)
       || NotInt(a+1) || NotInt(a+2) )
    return errWrongType;

  QImage *img = QIMAGE_FROM_OBJECT( slotRawObject(r) );
  QColor color( QC::read<QColor>(a) );
  int x = QC::read<int>(a+1);
  int y = QC::read<int>(a+2);

  if( x >= img->width() || y >= img->height() )
    return errIndexOutOfRange;

  QRgb *line = reinterpret_cast<QRgb*>( img->scanLine(y) );
  line[x] = color.rgba();

  return errNone;
}

QC_LANG_PRIMITIVE( QImage_TransferPixels, 4, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
    if (!isKindOfSlot(a, class_int32array)) {
        qcErrorMsg("QImage: array argument is not a Int32Array");
        return errWrongType;
    }

    if( NotInt(a+2) ) return errWrongType;
    int start = QC::read<int>(a+2);

    if( !(IsTrue(a+3) || IsFalse(a+3)) ) return errWrongType;
    bool store = IsTrue(a+3); // t/f g/s

    QImage *image = QIMAGE_FROM_OBJECT( slotRawObject(r) );
    QRect rect;

    if( IsNil(a+1) ) {
        rect = image->rect();
    }
    else {
        if (!isKindOfSlot(a+1, SC_CLASS(Rect)))
            return errWrongType;
        rect = QC::read<QRect>(a+1);
        if (rect.isEmpty())
            return errNone;
        if( !image->rect().contains(rect) ) {
            qcErrorMsg("QImage: source rectangle out of image bounds");
            return errFailed;
        }
    }

    PyrInt32Array* array = reinterpret_cast<PyrInt32Array*>( slotRawObject(a) );
    QRgb *pixelData = reinterpret_cast<QRgb*>( array->i ) + start;

    int width = rect.width();
    int height = rect.height();
    int size = width * height;
    int x = rect.x();
    int y = rect.y();
    int max_x = width + x;
    int max_y = height + y;

    if( array->size - start < size )
        return errIndexOutOfRange;

    if(store) {
        for( int iy = y; iy < max_y; ++iy ) {
            QRgb *line = reinterpret_cast<QRgb*>( image->scanLine(iy) );
            for( int ix = x; ix < max_x; ++ix ) {
                line[ix] = *pixelData;
                ++pixelData;
            }
        }
    } else {
        for( int iy = y; iy < max_y; ++iy ) {
            QRgb *line = reinterpret_cast<QRgb*>( image->scanLine(iy) );
            for( int ix = x; ix < max_x; ++ix ) {
                *pixelData = line[ix];
                ++pixelData;
            }
        }
    }

    return errNone;
}

QC_LANG_PRIMITIVE( QImage_Fill, 1, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  if( NotObj(a) || slotRawObject(a)->classptr != SC_CLASS(Color) )
      return errWrongType;

  QColor color = QC::read<QColor>(a);
  QImage *img = QIMAGE_FROM_OBJECT( slotRawObject(r) );
  img->fill(color);

  return errNone;
}

QC_LANG_PRIMITIVE( QImage_Formats, 1, PyrSlot *r, PyrSlot *a, VMGlobals *g )
{
  if( NotInt(a) ) return errWrongType;
  int rw = QC::read<int>(a);

  QList<QByteArray> formats;
  if( rw ) {
      formats = QImageWriter::supportedImageFormats();
  } else {
      formats = QImageReader::supportedImageFormats();
  }

  PyrObject *arr = instantiateObject( g->gc, s_array->u.classobj, formats.size(), true, true );
  SetObject(r, arr);

  for( int i = 0; i < formats.size(); i++ ) {
    QString qstr = QString(formats.at(i));
    PyrString *str = newPyrString( g->gc, qstr.toStdString().c_str(), obj_immutable, false );
    SetObject(arr->slots+i, str);
  }

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
  definer.define<QImage_SetSize>();
  definer.define<QImage_Write>();
  definer.define<QImage_SetPainter>();
  definer.define<QImage_UnsetPainter>();
  definer.define<QImage_GetPixel>();
  definer.define<QImage_GetColor>();
  definer.define<QImage_SetPixel>();
  definer.define<QImage_SetColor>();
  definer.define<QImage_TransferPixels>();
  definer.define<QImage_Fill>();
  definer.define<QImage_Formats>();
}

} // namespace QtCollider
