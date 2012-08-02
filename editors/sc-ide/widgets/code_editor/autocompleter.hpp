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

#ifndef SCIDE_WIDGETS_CODE_EDITOR_AUTOCOMPLETER_HPP_INCLUDED
#define SCIDE_WIDGETS_CODE_EDITOR_AUTOCOMPLETER_HPP_INCLUDED

#include <QObject>
#include <QTextDocument>
#include <QStack>
#include <QKeyEvent>
#include <QStringList>
#include <QSortFilterProxyModel>
#include <QPointer>

namespace ScIDE {

class CodeEditor;
class TokenIterator;
class ScRequest;
class CompletionMenu;
class MethodCallWidget;

class AutoCompleter : public QObject
{
    Q_OBJECT

public:
    struct Method {
        QString className;
        QString methodName;
        QStringList argNames;
        QStringList argDefaults;
    };

    AutoCompleter( CodeEditor * );


    void keyPress( QKeyEvent * );
    void documentChanged( QTextDocument * );

private slots:
    void onContentsChange(int pos, int removed, int added);
    void onCursorChanged();
    void onCompletionResponse( const QString & cmd, const QString & data );
    void onMethodCallResponse( const QString & cmd, const QString & data );
    void onCompletionMenuFinished( int result );

private:
    struct MethodCall {
        int position;
        Method method;
    };

    enum CompletionType {
        ClassCompletion,
        ClassMethodCompletion,
        MethodCompletion
    };

    typedef QStack<MethodCall>::iterator MethodCallIterator;

    QTextDocument *document();

    // completion

    void startCompletion();
    void quitCompletion( const QString & reason = QString() );
    void onCompletionResponse( const QString & data );
    void updateCompletionMenu();

    // method call aid

    void startMethodCall();
    void updateMethodCall( int cursorPos );
    void onMethodCallResponse( const QString & data );
    void pushMethodCall( const MethodCall & call );
    void showMethodCall( const MethodCall & call, int arg = 0 );
    void hideMethodCall();

    // utilities

    QString tokenText( TokenIterator & it );

    // data

    CodeEditor *mEditor;
    ScRequest *mCompletionRequest;
    ScRequest *mMethodCallRequest;

    struct {
        bool on;
        CompletionType type;
        int pos;
        int len;
        int contextPos;
        QString text;
        QPointer<CompletionMenu> menu;
    } mCompletion;

    struct {
        int pos;
        QStack<MethodCall> stack;
        QPointer<MethodCallWidget> widget;
    } mMethodCall;
};

} // namespace ScIDE

Q_DECLARE_METATYPE(ScIDE::AutoCompleter::Method);

#endif
