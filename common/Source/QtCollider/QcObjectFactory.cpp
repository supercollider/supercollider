/************************************************************************
*
* Copyright 2010 Jakob Leben (jakob.leben@gmail.com)
*
* This file is part of SuperCollider Qt GUI.
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

#include "QcObjectFactory.h"

#include <QMap>

#include <cstdio>

static QMap<QString, CreateFn> factories;

CreateFn factoryFunction( const QString& scClassName )
{
  return factories.value( scClassName, 0 );
}

QcObjectFactory::QcObjectFactory( const QString& scClassName, CreateFn fn )
{
  printf("QtCollider: declaring %s\n", scClassName.toStdString().c_str() );
  factories.insert( scClassName, fn );
}

#include "QObjectDeclarations.cpp"
