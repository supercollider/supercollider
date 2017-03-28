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

#include "sc_introspection.hpp"
#include "util/standard_dirs.hpp"
#include "../widgets/main_window.hpp"

#include <yaml-cpp/yaml.h>

#include <cassert>

#include <QDebug>
#include <QHash>
#include <QObject>

namespace ScIDE {
namespace ScLanguage {

Introspection::Introspection()
{
    initPaths();
}

Introspection::Introspection( QString const & yamlString )
{
    initPaths();
    bool parsingSuccessful = parse(yamlString);
    if (!parsingSuccessful)
        throw std::runtime_error("Introspection parse error");
}

Introspection & Introspection::operator =(Introspection && rhs)
{
    mClassMap           = std::move( rhs.mClassMap           );
    mMethodMap          = std::move( rhs.mMethodMap          );
    mClassLibraryPath   = std::move( rhs.mClassLibraryPath   );
    mUserExtensionDir   = std::move( rhs.mUserExtensionDir   );
    mSystemExtensionDir = std::move( rhs.mSystemExtensionDir );
    return *this;
}

void Introspection::initPaths()
{
    mUserExtensionDir = standardDirectory(ScExtensionUserDir) + QStringLiteral("/");
    mSystemExtensionDir = standardDirectory(ScExtensionSystemDir) + QStringLiteral("/");
}

bool Introspection::parse(const QString & yamlString )
{
    using std::make_pair;

    clear();

    const YAML::Node doc = YAML::Load( yamlString.toStdString() );
    if( !doc ) {
        MainWindow::instance()->showStatusMessage("no YAML document");
        return false;
    }

    assert(doc.IsSequence());
    for( auto const & element : doc )
    {
        assert(element.IsSequence());
        QString name = element[0].as<std::string>().c_str();
        Class *klass = new Class;
        klass->name = name;
        mClassMap.insert(make_pair(klass->name, QSharedPointer<Class>(klass)));
    }

    for( YAML::Node const & node : doc )
    {
        QString name = node[0].as<std::string>().c_str();
        ClassMap::iterator it = mClassMap.find(name);
        assert(it != mClassMap.end());
        Class *klass = it->second.data();

        //qDebug() << klass->name;

        ClassMap::iterator class_it;

        QString metaClassName = node[1].as<std::string>().c_str();
        class_it = mClassMap.find(metaClassName);
        assert(class_it != mClassMap.end());
        klass->metaClass = class_it->second.data();

        if (node[2].IsNull())
            klass->superClass = 0;
        else {
            QString superClassName = node[2].as<std::string>().c_str();
            class_it = mClassMap.find(superClassName);
            assert(class_it != mClassMap.end());
            klass->superClass = class_it->second.data();
        }

        klass->definition.path = node[3].as<std::string>().c_str();
        klass->definition.position = node[4].as<int>();

        const YAML::Node &methodSeq = node[5];
        if (methodSeq.Type() != YAML::NodeType::Sequence)
            continue;

        //assert(methodSeq.Type() == YAML::NodeType::Sequence);
        for( const YAML::Node &methodNode : methodSeq )
        {
            assert(methodNode.Type() == YAML::NodeType::Sequence);
            assert(methodNode.size() >= 2);

            assert(methodNode[0].Type() == YAML::NodeType::Scalar);
            assert(methodNode[1].Type() == YAML::NodeType::Scalar);

            Method *method = new Method;
            method->ownerClass = klass;
            method->name = methodNode[1].as<std::string>().c_str();
            method->definition.path = methodNode[2].as<std::string>().c_str();
            method->definition.position = methodNode[3].as<int>();

            //qDebug() << "--" << method->name;

            const YAML::Node &argNode = methodNode[4];
            assert(argNode.Type() == YAML::NodeType::Sequence);
            auto arg = argNode.begin();
            while (arg != argNode.end())
            {
                Argument argument;

                // get arg name
                assert(arg->Type() == YAML::NodeType::Scalar);
                argument.name = arg->as<std::string>().c_str();

                //qDebug() << "---# " << argument.name;

                // get arg default value
                ++arg;
                if (arg == argNode.end())
                    break;
                if(!arg->IsNull()) {
                    assert(arg->Type() == YAML::NodeType::Scalar);
                    argument.defaultValue = arg->as<std::string>().c_str();
                }

                method->arguments.append(argument);

                // next arg
                ++arg;
            }

            klass->methods.append(method);
            mMethodMap.insert(make_pair(method->name, QSharedPointer<Method>(method)));
        }

        qSort(klass->methods.begin(), klass->methods.end(), [](Method * lhs, Method * rhs) {
            return lhs->name.get() < rhs->name.get();
        });
    }

    inferClassLibraryPath();

    //qDebug("done parsing introspection.");
    return true;
}

QString Introspection::compactLibraryPath(QString const & path) const
{
    if (path.startsWith(mClassLibraryPath))
        return path.mid(mClassLibraryPath.length());

    if (path.startsWith(mUserExtensionDir))
        return QStringLiteral("Extensions/") + path.mid(mUserExtensionDir.length());

    if (path.startsWith(mSystemExtensionDir))
        return QStringLiteral("Extensions/") + path.mid(mSystemExtensionDir.length());

    return path;
}

void Introspection::inferClassLibraryPath()
{
    ClassMap::const_iterator object_class_it = mClassMap.find("Object");
    assert(object_class_it != mClassMap.end());
    Class *objectClass = object_class_it->second.data();

    QString classLibPath = objectClass->definition.path;
    int len = classLibPath.lastIndexOf("Common");
    if (len != -1)
        classLibPath.truncate(len);
    else
        classLibPath.clear();

    mClassLibraryPath = classLibPath;
}

bool Introspection::ensureIntrospectionData() const
{
    if (!introspectionAvailable()) {
        MainWindow::instance()->showStatusMessage(
                    QObject::tr("Sclang Introspection not available, yet!"));
        return false;
    } else
        return true;
}

const Class * Introspection::findClass(const QString &className) const
{
    if (!ensureIntrospectionData())
        return NULL;

    ClassMap::const_iterator klass_it = mClassMap.find(className);
    if (klass_it == mClassMap.end()) {
        MainWindow::instance()->showStatusMessage(
                    QObject::tr("Class not defined!"));
        return NULL;
    }
    return klass_it->second.data();
}

std::vector<const Class*> Introspection::findClassPartial(const QString & partialClassName) const
{
    std::vector<const Class*> matchingClasses;
    if (!ensureIntrospectionData())
        return matchingClasses;

    typedef ClassMap::const_iterator class_iterator;

    for (class_iterator it = mClassMap.begin(); it != mClassMap.end(); ++it) {
        QString const & key = it->first;
        if (key.contains(partialClassName, Qt::CaseInsensitive)) {
            if (!key.startsWith("Meta_"))
                matchingClasses.push_back(it->second.data());
        }
    }

    return matchingClasses;
}


std::vector<const Method*> Introspection::findMethodPartial(const QString & partialMethodName) const
{
    std::vector<const Method*> matchingMethods;
    if (!ensureIntrospectionData())
        return matchingMethods;

    typedef MethodMap::const_iterator class_iterator;

    for (class_iterator it = mMethodMap.begin(); it != mMethodMap.end(); ++it) {
        QString const & key = it->first;
        if (key.contains(partialMethodName, Qt::CaseInsensitive))
            matchingMethods.push_back(it->second.data());
    }

    return matchingMethods;
}

Introspection::ClassMethodMap Introspection::constructMethodMap(const Class * klass) const
{
    ClassMethodMap methodMap;
    if (!klass)
        return methodMap;

    foreach (Method *method, klass->metaClass->methods) {
        QList<Method*> & list = methodMap[method->definition.path];
        list.append(method);
    }

    foreach (Method *method, klass->methods) {
        QList<Method*> & list = methodMap[method->definition.path];
        list.append(method);
    }
    return methodMap;
}
  
bool Method::matches(const QString& toMatch) const
{
    return toMatch.isEmpty() ? true : name.get().startsWith(toMatch, Qt::CaseInsensitive);
}


QString Method::signature( SignatureStyle style ) const
{
    QString sig = ownerClass->name.get();
    if (sig.startsWith("Meta_")) {
        sig.remove(0, 5);
        sig.append(": *");
    }
    else
        sig.append(": ");

    sig.append(name.get());

    if (style == SignatureWithoutArguments)
        return sig;

    int argc = arguments.count();
    if (argc) {
        sig.append(" (");
        for( int i = 0; i < argc; ++i )
        {
            const Argument & arg = arguments[i];
            if (i > 0)
                sig.append(", ");
            sig.append(arg.name);
            if (style == SignatureWithArgumentsAndDefaultValues && !arg.defaultValue.get().isEmpty()) {
                sig.append(" = ");
                sig.append(arg.defaultValue);
            }
        }
        sig.append(")");
    }
    else if (name.get().endsWith('_'))
        sig.append(" (value)");

    return sig;
}

} // namespace ScLanguage
} // namespace ScIDE
