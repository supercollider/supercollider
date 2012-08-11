/*
    SuperCollider Qt IDE
    Copyright (c) 2012 Jakob Leben & Tim Blechmann
    http://www.audiosynth.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

#ifndef SCIDE_CORE_SC_INTROSPECTION_HPP_INCLUDED
#define SCIDE_CORE_SC_INTROSPECTION_HPP_INCLUDED

#include <QString>
#include <QVector>
#include <QMetaType>

#include <map>

namespace ScIDE {

namespace ScLanguage {

struct Class;
struct Method;
struct Argument;

typedef std::map<QString, Class*> ClassMap;
typedef std::multimap<QString, Method*> MethodMap;
typedef QVector<Argument> ArgumentVector;
typedef QVector<Method*> MethodVector;

struct Argument {
    QString name;
    QString defaultValue;
};

struct Class {
    QString name;
    Class *metaClass;
    Class *superClass;
    MethodVector methods;
    struct {
        QString path;
        int position;
    } definition;
};

struct Method {
    Class *ownerClass;
    QString name;
    ArgumentVector arguments;
    struct {
        QString path;
        int position;
    } definition;
};

class Introspection
{
public:
    bool parse(const QString & yamlString );

    const ClassMap & classMap() const { return mClassMap; }
    const MethodMap & methodMap() const { return mMethodMap; }

private:
    ClassMap mClassMap;
    MethodMap mMethodMap;
};

} // namespace ScLanguage

} // namespace ScIDE

Q_DECLARE_METATYPE(ScIDE::ScLanguage::Class*)
Q_DECLARE_METATYPE(const ScIDE::ScLanguage::Method*)

#endif // SCIDE_CORE_SC_INTROSPECTION_HPP_INCLUDED
