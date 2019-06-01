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

#pragma once

#include <QHash>
#include <QList>
#include <QMap>
#include <QMetaType>
#include <QSharedPointer>
#include <QString>
#include <QVector>

#include <string>
#include <map>

#include <boost/flyweight.hpp>


static inline std::size_t hash_value(QString const& b) { return qHash(b); }

namespace ScIDE { namespace ScLanguage {

typedef boost::flyweight<QString> FlyweightString;

typedef std::map<QString, QSharedPointer<struct Class>> ClassMap;
typedef std::multimap<QString, QSharedPointer<struct Method>> MethodMap;
typedef QVector<struct Argument> ArgumentVector;
typedef QVector<struct Method*> MethodVector;

struct Argument {
    FlyweightString name;
    FlyweightString defaultValue;
};

struct Class {
    FlyweightString name;
    Class* metaClass;
    Class* superClass;
    MethodVector methods;
    struct {
        FlyweightString path;
        int position;
    } definition;

    bool isSubclassOf(const Class* parentClass) const {
        if (superClass == parentClass)
            return true;

        if (superClass == NULL)
            return false;

        return superClass->isSubclassOf(parentClass);
    }
};

struct Method {
    enum SignatureStyle { SignatureWithoutArguments, SignatureWithArguments, SignatureWithArgumentsAndDefaultValues };

    QString signature(SignatureStyle style) const;
    bool matches(const QString& toMatch) const;

    Class* ownerClass;
    FlyweightString name;
    ArgumentVector arguments;
    struct {
        FlyweightString path;
        int position;
    } definition;
};

static inline QString makeFullMethodName(QString const& className, QString const& methodName) {
    QString ret(className);
    if (ret.startsWith("Meta_")) {
        ret.remove(0, 5);
        ret.append("-*");
    } else
        ret.append("-");
    ret.append(methodName);
    return ret;
}

class Introspection {
public:
    Introspection();
    Introspection(Introspection const& rhs) = default;
    explicit Introspection(QString const& yamlString);

    Introspection& operator=(Introspection const& rhs) = default;
    Introspection& operator=(Introspection&& rhs);

    typedef QMap<QString, QList<Method*>> ClassMethodMap; // maps Path to List of Methods

    const ClassMap& classMap() const { return mClassMap; }
    const MethodMap& methodMap() const { return mMethodMap; }

    const Class* findClass(QString const& className) const;
    std::vector<const Class*> findClassPartial(const QString& partialClassName) const;
    std::vector<const Method*> findMethodPartial(const QString& partialMethodName) const;

    ClassMethodMap constructMethodMap(const Class* klass) const;
    ClassMethodMap constructMethodMap(QString const& className) const {
        return constructMethodMap(findClass(className));
    }

    QString const& classLibraryPath() const { return mClassLibraryPath; }

    // remove class library path, userExtensionDir and systemExtensionDir
    QString compactLibraryPath(QString const& path) const;

    bool isClassMethod(const Method* method) const { return (method->ownerClass->name.get().startsWith("Meta_")); }

    bool introspectionAvailable() const { return !mClassMap.empty(); }

private:
    void initPaths();
    bool parse(const QString& yamlString);
    void inferClassLibraryPath();
    bool ensureIntrospectionData() const;

    void clear() {
        mClassMap.clear();
        mMethodMap.clear();
        mClassLibraryPath.clear();
    }

    ClassMap mClassMap;
    MethodMap mMethodMap;
    QString mClassLibraryPath;
    QString mUserExtensionDir;
    QString mSystemExtensionDir;
};

} // namespace ScLanguage

} // namespace ScIDE

Q_DECLARE_METATYPE(ScIDE::ScLanguage::Class*)
Q_DECLARE_METATYPE(const ScIDE::ScLanguage::Method*)
