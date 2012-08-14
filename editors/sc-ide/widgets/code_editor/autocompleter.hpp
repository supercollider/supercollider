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

namespace ScLanguage { struct Method; }

class AutoCompleter : public QObject
{
    Q_OBJECT

public:

    AutoCompleter( CodeEditor * );

    void keyPress( QKeyEvent * );
    void documentChanged( QTextDocument * );
    void triggerCompletion();
    // NOTE: default 'force' to true for the purpose of
    // argument-less QAction::triggered() signal
    void triggerMethodCallAid( bool force = true );

private slots:
    void onContentsChange(int pos, int removed, int added);
    void onCursorChanged();
    void onCompletionMenuFinished( int result );

private:
    struct MethodCall {
        MethodCall(): position(0), method(0) {}
        int position;
        const ScLanguage::Method *method;
    };

    enum CompletionType {
        ClassCompletion,
        ClassMethodCompletion,
        MethodCompletion
    };

    typedef QStack<MethodCall>::iterator MethodCallIterator;

    QTextDocument *document();

    // completion

    void quitCompletion( const QString & reason = QString() );
    void showCompletionMenu();
    void updateCompletionMenu();

    // method call aid

    void updateMethodCall( int cursorPos );
    void pushMethodCall( const MethodCall & call );
    void showMethodCall( const MethodCall & call, int arg = 0 );
    void hideMethodCall();

    // utilities

    QString tokenText( TokenIterator & it );

    // data

    CodeEditor *mEditor;

    struct {
        bool on;
        CompletionType type;
        int pos;
        int len;
        int contextPos;
        QString base;
        QString text;
        QPointer<CompletionMenu> menu;
    } mCompletion;

    struct {
        QStack<MethodCall> stack;
        QPointer<CompletionMenu> menu;
        QPointer<MethodCallWidget> widget;
    } mMethodCall;
};

} // namespace ScIDE

#endif
