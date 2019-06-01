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

#include <QObject>
#include <QTextDocument>
#include <QStack>
#include <QKeyEvent>
#include <QStringList>
#include <QPointer>

#include "tokens.hpp"
#include "completion_menu.hpp"

#include "SCDoc/SCDoc.h"

namespace ScIDE {

class ScCodeEditor;
class TokenIterator;
class MethodCallWidget;

namespace ScLanguage {
struct Method;
struct Class;
}

class AutoCompleter : public QObject {
    Q_OBJECT

public:
    AutoCompleter(ScCodeEditor*);

    void keyPress(QKeyEvent*);
    void documentChanged(QTextDocument*);
    // NOTE: default 'force' to true in the following methods
    // for the purpose of argument-less QAction::triggered() signal:
    void triggerCompletion(bool forceShow = true);
    void triggerMethodCallAid(bool explicitly = true);

private slots:
    void onContentsChange(int pos, int removed, int added);
    void onCursorChanged();
    void onCompletionMenuFinished(int result);
    void updateCompletionMenuInfo();
    void clearMethodCallStack();
    void hideWidgets();
    void gotoHelp(QString symbol);

private:
    friend class MethodCallWidget;

    struct MethodCall {
        MethodCall(): position(0), method(0), functionalNotation(false), suppressed(false) {}
        int position;
        const ScLanguage::Method* method;
        bool functionalNotation;
        bool suppressed;
    };

    enum CompletionType { ClassCompletion, ClassMethodCompletion, MethodCompletion, InvalidCompletion };

    struct CompletionDescription {
        bool on;
        CompletionType type;
        int pos;
        int len;
        int contextPos;
        QString base;
        QString text;
        QPointer<CompletionMenu> menu;
        Token::Type tokenType;
    };

    struct MethodCallContext {
        QStack<MethodCall> stack;
        QPointer<CompletionMenu> menu;
        QPointer<MethodCallWidget> widget;
    };

    typedef QStack<MethodCall>::iterator MethodCallIterator;

    bool eventFilter(QObject*, QEvent*);

    QTextDocument* document();

    // completion

    void quitCompletion(const QString& reason = QString());
    void showCompletionMenu(bool forceShow);
    void updateCompletionMenu(bool forceShow);

    // method call aid

    const ScLanguage::Method* disambiguateMethod(const QString& methodName, int cursorPos);
    void updateMethodCall(int cursorPos);
    void pushMethodCall(const MethodCall& call);
    void showMethodCall(const MethodCall& call, int arg = 0);
    void hideMethodCall();
    bool trySwitchMethodCallArgument(bool backwards = false);
    bool testMethodCall(const MethodCall&, int cursorPos, int& argNum, TokenIterator& argNameToken,
                        bool strict = false);

    static CompletionMenu* menuForClassCompletion(CompletionDescription const& completion, ScCodeEditor* editor);
    static CompletionMenu* menuForClassMethodCompletion(CompletionDescription const& completion, ScCodeEditor* editor);
    static CompletionMenu* menuForMethodCompletion(CompletionDescription const& completion, ScCodeEditor* editor);

    static const ScLanguage::Class* classForToken(Token::Type type, const QString& string);

    // utilities

    QString tokenText(TokenIterator& it);
    QRect globalCursorRect(int cursorPosition);
    static QString findHelpClass(QString klass);
    static DocNode* parseHelpClass(QString file);
    static QString parseClassElement(DocNode* node, QString element);
    static void parseClassNode(DocNode* node, QString* str);

    // data

    ScCodeEditor* mEditor;
    CompletionDescription mCompletion;
    MethodCallContext mMethodCall;
};

} // namespace ScIDE
