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

#include "yaml-cpp/node.h"
#include "yaml-cpp/parser.h"

#include <cassert>

#include <QDebug>
#include <QHash>

namespace ScIDE {
namespace ScLanguage {

bool Introspection::parse(const QString & yamlString )
{
    using std::make_pair;

    clear();

    //qDebug("parsing introspection...");

    std::stringstream stream;
    stream << yamlString.toStdString();
    YAML::Parser parser(stream);

    YAML::Node doc;
    if(!parser.GetNextDocument(doc)) {
        qWarning("no YAML document");
        return false;
    }

    assert (doc.Type() == YAML::NodeType::Sequence);
    for (YAML::Iterator it = doc.begin(); it != doc.end(); ++it)
    {
        assert(it->Type() == YAML::NodeType::Sequence);
        QString name = (*it)[0].to<std::string>().c_str();
        Class *klass = new Class;
        klass->name = name;
        mClassMap.insert(make_pair(klass->name, klass));
    }

    for (YAML::Iterator docIterator = doc.begin(); docIterator != doc.end(); ++docIterator)
    {
        const YAML::Node & node = *docIterator;
        QString name = node[0].to<std::string>().c_str();
        ClassMap::iterator it = mClassMap.find(name);
        assert(it != mClassMap.end());
        Class *klass = it->second;

        //qDebug() << klass->name;

        ClassMap::iterator class_it;

        QString metaClassName = node[1].to<std::string>().c_str();
        class_it = mClassMap.find(metaClassName);
        assert(class_it != mClassMap.end());
        klass->metaClass = class_it->second;

        if (node[2].Read(YAML::Null))
            klass->superClass = 0;
        else {
            QString superClassName = node[2].to<std::string>().c_str();
            class_it = mClassMap.find(superClassName);
            assert(class_it != mClassMap.end());
            klass->superClass = class_it->second;
        }

        klass->definition.path = node[3].to<std::string>().c_str();
        klass->definition.position = node[4].to<int>();

        const YAML::Node &methodSeq = node[5];
        if (methodSeq.Type() != YAML::NodeType::Sequence)
            continue;

        //assert(methodSeq.Type() == YAML::NodeType::Sequence);
        for (YAML::Iterator mit = methodSeq.begin(); mit != methodSeq.end(); ++mit)
        {
            const YAML::Node &methodNode = *mit;
            assert(methodNode.Type() == YAML::NodeType::Sequence);
            assert(methodNode.size() >= 2);

            assert(methodNode[0].Type() == YAML::NodeType::Scalar);
            assert(methodNode[1].Type() == YAML::NodeType::Scalar);

            Method *method = new Method;
            method->ownerClass = klass;
            method->name = methodNode[1].to<std::string>().c_str();
            method->definition.path = methodNode[2].to<std::string>().c_str();
            method->definition.position = methodNode[3].to<int>();

            //qDebug() << "--" << method->name;

            const YAML::Node &argNode = methodNode[4];
            assert(argNode.Type() == YAML::NodeType::Sequence);
            YAML::Iterator arg = argNode.begin();
            while (arg != argNode.end())
            {
                Argument argument;

                // get arg name
                assert(arg->Type() == YAML::NodeType::Scalar);
                argument.name = arg->to<std::string>().c_str();

                //qDebug() << "---# " << argument.name;

                // get arg default value
                ++arg;
                if (arg == node.end())
                    break;
                if(!arg->Read(YAML::Null)) {
                    assert(arg->Type() == YAML::NodeType::Scalar);
                    argument.defaultValue = arg->to<std::string>().c_str();
                }

                method->arguments.append(argument);

                // next arg
                ++arg;
            }

            klass->methods.append(method);
            mMethodMap.insert(make_pair(method->name, method));
        }
    }

    //qDebug("done parsing introspection.");
    return true;
}

} // namespace ScLanguage
} // namespace ScIDE
