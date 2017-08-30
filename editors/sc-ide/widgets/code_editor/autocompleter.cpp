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

#define QT_NO_DEBUG_OUTPUT

#include "autocompleter.hpp"
#include "sc_editor.hpp"
#include "tokens.hpp"
#include "../../core/sc_introspection.hpp"
#include "../../core/sc_process.hpp"
#include "../../core/main.hpp"
#include "../../core/util/standard_dirs.hpp"
#include "../main_window.hpp"
#include "../help_browser.hpp"

#include <yaml-cpp/node/node.h>
#include <yaml-cpp/parser.h>

#include <QDebug>
#include <QLabel>
#include <QScrollBar>
#include <QApplication>
#include <QDesktopWidget>
#include <QProxyStyle>

namespace ScIDE {

static bool tokenMaybeName( Token::Type type )
{
    return (type == Token::Name || type == Token::Keyword || type == Token::Builtin);
}

static QString incrementedString( const QString & other )
{
    if(other.isEmpty())
        return QString();

    QString str = other;
    int pos = str.length()-1;
    str[pos] = QChar( str[pos].unicode() + 1 );
    return str;
}

class MethodCallWidget : public QWidget
{
public:
    MethodCallWidget( QWidget * parent = 0 ):
        QWidget( parent, Qt::ToolTip )
    {
        mLabel = new QLabel(this);
        mLabel->setTextFormat( Qt::RichText );
        mLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

// Qt 4 had a class called "QGtkStyle" which allowed Qt to access the
// native style of the window manager in Linux and other *nix systems.
// It was removed in Qt 5, so we don't have that option anymore. We have
// to hardcode the colors now. In the future, this should be configurable
// by the user.

// This #if used to be "#if defined(Q_WS_X11)", and then NetBSD was
// excluded (commit c3017f5) because QGtkStyle was not defined on that
// system. Qt 5 got rid of the Q_WS_ macros, so we changed Q_WS_X11 to
// Q_OS_UNIX && !Q_OS_MAC as a best guess. The NetBSD check is probably
// vestigial, but we REALLY needed to get this fix through since Linux
// users have had unreadable autocomplete widgets for almost 18 months
// now. We figured it was best to leave it alone.

// See: https://github.com/supercollider/supercollider/pull/2762

#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC) && !defined(__NetBSD__)
        QPalette p;
        p.setColor( QPalette::Window, QColor(255, 255, 220) );
        p.setColor( QPalette::WindowText, Qt::black );
        setPalette(p);
#else
        QPalette p( palette() );
        p.setColor( QPalette::Window, p.color(QPalette::ToolTipBase) );
        setPalette(p);
        mLabel->setForegroundRole(QPalette::ToolTipText);
#endif
    }

    void showMethod( const AutoCompleter::MethodCall & methodCall,
                     int argNum, const QRect & cursorRect )
    {
        const ScLanguage::Method *method = methodCall.method;
        int argc = method->arguments.count();

        QString text;

        if (methodCall.functionalNotation) {
            addArgument(text, "receiver", QString(), argNum == 0 );
            --argNum;
            if (argc)
                text += ", &nbsp;&nbsp;";
        }

        for (int i = 0; i < argc; ++i)
        {
            const ScLanguage::Argument & arg = method->arguments[i];

            addArgument(text, arg.name, arg.defaultValue, argNum == i);

            if (i != argc - 1)
                text += ", &nbsp;&nbsp;";
        }

        mLabel->setText(text);

        mTargetRect = cursorRect;

        updateGeometry();
        show();
    }

private:
    void static addArgument ( QString & text, const QString & argText, const QString & valText, bool highlight )
    {
        if (highlight) {
            text += QString(
                "<span style=\""
                //"text-decoration: underline;"
                "font-weight: bold;"
                "\">");
        }

        text += argText;

        if (!valText.isEmpty())
            text += " = " + valText;

        if (highlight)
            text += "</span>";
    }

    void updateGeometry() {
        static const QSize margins = QSize(5,2);

        QSize labelSize = mLabel->sizeHint();
        mLabel->move(margins.width(), margins.height());
        mLabel->resize(labelSize);

        QRect rect;
        rect.setSize( labelSize  + (margins * 2) );
        rect.moveBottomLeft( mTargetRect.topLeft() );

        QWidget * parentWid = parentWidget();
        QWidget * referenceWidget = parentWid ? parentWid : this;

        QRect screen = QApplication::desktop()->availableGeometry(referenceWidget);
        if (!screen.contains(rect))
        {
            if (rect.right() > screen.right())
                rect.moveRight( screen.right() );
            if (rect.left() < screen.left())
                rect.moveLeft( screen.left() );
            if (rect.top() < screen.top())
                rect.moveTop( qMax( mTargetRect.bottom(), screen.top() ) );
            if (rect.bottom() > screen.bottom())
                rect.moveBottom( screen.bottom() );
        }

        setGeometry(rect);
    }

    QLabel *mLabel;
    QRect mTargetRect;
};

AutoCompleter::AutoCompleter( ScCodeEditor *editor ):
    QObject(editor),
    mEditor(editor)
{
    mCompletion.on = false;
    mEditor->installEventFilter(this);

    connect(editor, SIGNAL(cursorPositionChanged()),
            this, SLOT(onCursorChanged()));
    connect( editor->horizontalScrollBar(), SIGNAL(valueChanged(int)),
             this, SLOT(hideWidgets()) );
    connect( editor->verticalScrollBar(), SIGNAL(valueChanged(int)),
             this, SLOT(hideWidgets()) );
    connect(Main::scProcess(), SIGNAL(introspectionChanged()),
            this, SLOT(clearMethodCallStack()));
}

void AutoCompleter::documentChanged( QTextDocument * doc )
{
    connect(doc, SIGNAL(contentsChange(int,int,int)),
            this, SLOT(onContentsChange(int,int,int)));
}

inline QTextDocument *AutoCompleter::document()
{
    return static_cast<QPlainTextEdit*>(mEditor)->document();
}

void AutoCompleter::keyPress( QKeyEvent *e )
{
    switch (e->key())
    {
    case Qt::Key_ParenLeft:
    case Qt::Key_Comma:
        triggerMethodCallAid(false);
        break;
    case Qt::Key_Backspace:
    case Qt::Key_Delete:
        return;
    default:
        qDebug(">>> key");
        // Only trigger completion if event produces at least 1 printable character:
        if (!mCompletion.on && !e->text().isEmpty() && e->text()[0].isPrint() )
            triggerCompletion();
    }
}

bool AutoCompleter::eventFilter( QObject *object, QEvent *event )
{
    if (object != mEditor)
        return false;

    switch(event->type()) {
    case QEvent::FocusOut:
        hideWidgets();
        break;
    case QEvent::ShortcutOverride: {
        QKeyEvent * kevent = static_cast<QKeyEvent*>(event);
        switch(kevent->key()) {
        case Qt::Key_Left:
        case Qt::Key_Right:
            if (mCompletion.menu && mCompletion.menu->isVisible())
                mCompletion.menu->reject();
            break;
        case Qt::Key_Escape:
            if (mCompletion.menu && mCompletion.menu->isVisible())
                mCompletion.menu->reject();
            else if (mMethodCall.menu && mMethodCall.menu->isVisible())
                mMethodCall.menu->reject();
            else if (mMethodCall.widget && mMethodCall.widget->isVisible()) {
                // disable method call aid for current call:
                Q_ASSERT(!mMethodCall.stack.isEmpty());
                mMethodCall.stack.top().suppressed = true;
                hideMethodCall();
            }
            else break;
            return true;
        }
        break;
    }
    case QEvent::KeyPress: {
        QKeyEvent * kevent = static_cast<QKeyEvent*>(event);
        switch(kevent->key()) {
        case Qt::Key_Tab:
        case Qt::Key_Backtab:
            if( trySwitchMethodCallArgument(kevent->key() == Qt::Key_Backtab) ) {
                event->accept();
                return true;
            }
            break;
        }
        break;
    }
    default:;
    }

    return QObject::eventFilter(object, event);
}

void AutoCompleter::onContentsChange( int pos, int removed, int added )
{
    qDebug() << ">>> contentsChange:" << pos << "-" << removed << "+" << added;

    int callIdx = 0;
    while (callIdx < mMethodCall.stack.count())
    {
        MethodCall & call = mMethodCall.stack[callIdx];
        if (pos > call.position) {
            break;
        } else if (pos + removed > call.position) {
            qDebug("Method call: opening bracket deleted. popping.");
            mMethodCall.stack.remove(callIdx);
        } else {
            // Adjust method call positions.
            // FIXME: We are disregarding changes in context that defines the method call.
            // This is for reason of simplicity, and with the benefit that (irrelevant)
            // indentation changes don't destroy the method call stack
            qDebug("Method call: adjusting position: %i", call.position);
            call.position -= removed;
            call.position += added;
            qDebug("Method call: adjusted position: %i", call.position);
            ++callIdx;
        }
    }

    if (mCompletion.on)
    {
        if(pos < mCompletion.contextPos)
        {
            quitCompletion("context changed");
        }
        else if(pos <= mCompletion.pos + mCompletion.len)
        {
            QTextBlock block( document()->findBlock(mCompletion.pos) );
            TokenIterator it( block, mCompletion.pos - block.position() );
            Token::Type type = it.type();
            if (type == Token::Class || tokenMaybeName(type)) {
                mCompletion.len = it->length;
                mCompletion.text = tokenText(it);
            }
            else {
                mCompletion.len = 0;
                mCompletion.text.clear();
            }
            if (!mCompletion.menu.isNull())
                updateCompletionMenu(false);
        }
    }
}

void AutoCompleter::onCursorChanged()
{
    int cursorPos = mEditor->textCursor().position();
    qDebug(">>> cursorChanged: %i", cursorPos);

    // completion
    if (mCompletion.on) {
        if (cursorPos < mCompletion.pos ||
            cursorPos > mCompletion.pos + mCompletion.len)
        {
            quitCompletion("out of bounds");
        }
    }

    if (!mMethodCall.menu.isNull()) {
        qDebug("Method call: quitting menu");
        delete mMethodCall.menu;
    }

    updateMethodCall(cursorPos);
}

void AutoCompleter::triggerCompletion(bool forceShow)
{
    if (mCompletion.on) {
        qDebug("AutoCompleter::triggerCompletion(): completion already started.");
        updateCompletionMenu(forceShow);
        return;
    }

    QTextCursor cursor( mEditor->textCursor() );
    const int cursorPos = cursor.positionInBlock();
    QTextBlock block( cursor.block() );
    TokenIterator it( block, cursorPos - 1 );

    if (!it.isValid())
        return;

    const Token & triggeringToken = *it;

    if (triggeringToken.type == Token::Class)
    {
        if (triggeringToken.length < 3)
            return;
        mCompletion.type = ClassCompletion;
        mCompletion.pos = it.position();
        mCompletion.len = it->length;
        mCompletion.text = tokenText(it);
        mCompletion.contextPos = mCompletion.pos + 3;
        mCompletion.base = mCompletion.text;
        mCompletion.base.truncate(3);
    }
    else {
        TokenIterator objectIt, dotIt, nameIt;

        Token::Type objectTokenType = Token::Unknown;

        if (tokenMaybeName(it.type())) {
            nameIt = it;
            --it;
        }

        if (it.isValid() && it.character() == '.') {
            dotIt = it;
            --it;
        }
        else
            // don't trigger on method names without preceding dot (for now)
            return;

        if (dotIt.isValid()) {
            objectTokenType = it.type();
            switch (objectTokenType) {
            case Token::Class:
            case Token::Char:
            case Token::StringMark:
            case Token::Builtin:
            case Token::Symbol:
            case Token::SymbolMark:
            case Token::Float:
            case Token::RadixFloat:
            case Token::HexInt:
                objectIt = it;
                break;

            default:;
            }
        }

        if (!objectIt.isValid() && (!nameIt.isValid() || nameIt->length < 3))
            return;

        if (nameIt.isValid()) {
            mCompletion.pos = nameIt.position();
            mCompletion.len = nameIt->length;
            mCompletion.text = tokenText(nameIt);
        } else {
            mCompletion.pos = dotIt.position() + 1;
            mCompletion.len = 0;
            mCompletion.text.clear();
        }

        if (objectIt.isValid()) {
            mCompletion.contextPos = mCompletion.pos;
            mCompletion.base       = tokenText(objectIt);
            mCompletion.tokenType  = objectTokenType;
            mCompletion.type       = ClassMethodCompletion;
        }
        else {
            mCompletion.contextPos = mCompletion.pos + 3;
            mCompletion.base       = tokenText(nameIt);
            mCompletion.type       = MethodCompletion;
        }
    }

    mCompletion.on = true;

    qDebug() << QStringLiteral("Completion: ON <%1>").arg(mCompletion.base);

    showCompletionMenu(forceShow);

    if (mCompletion.menu.isNull())
        mCompletion.on = false;
}

void AutoCompleter::quitCompletion( const QString & reason )
{
    Q_ASSERT(mCompletion.on);

    qDebug() << QStringLiteral("Completion: OFF (%1)").arg(reason);

    if (mCompletion.menu) {
        mCompletion.menu->hide();
        mCompletion.menu->deleteLater();
        mCompletion.menu = 0;
    }

    mCompletion.on = false;
}

void AutoCompleter::showCompletionMenu(bool forceShow)
{
    qDebug(">>> showCompletionMenu");

    using namespace ScLanguage;
    using ScLanguage::Method;

    Q_ASSERT(mCompletion.on);
    Q_ASSERT(mCompletion.menu.isNull());

    QPointer<CompletionMenu> menu;

    switch (mCompletion.type) {
    case ClassCompletion:
        menu = menuForClassCompletion(mCompletion, mEditor);
        break;

    case ClassMethodCompletion:
        menu = menuForClassMethodCompletion(mCompletion, mEditor);
        break;

    case MethodCompletion:
        menu = menuForMethodCompletion(mCompletion, mEditor);
        break;

    default:
        break;
    }


    if (menu == NULL) return;

    mCompletion.menu = menu;

    connect(menu, SIGNAL(finished(int)), this, SLOT(onCompletionMenuFinished(int)));

    QRect popupTargetRect = globalCursorRect(mCompletion.pos).adjusted(0,-5,0,5);

    menu->popup( popupTargetRect );

    updateCompletionMenu(forceShow);

    if (mCompletion.type == ClassCompletion &&
            Main::settings()->value("IDE/editor/showAutocompleteHelp").toBool()) {
        connect(menu, SIGNAL(itemChanged(int)), this, SLOT(updateCompletionMenuInfo()));
        connect(menu, SIGNAL(infoClicked(QString)), this, SLOT(gotoHelp(QString)));
        updateCompletionMenuInfo();
    }
}

CompletionMenu * AutoCompleter::menuForClassCompletion(CompletionDescription const & completion,
                                                       ScCodeEditor * editor)
{
    using namespace ScLanguage;
    const Introspection & introspection = Main::scProcess()->introspection();

    const ClassMap & classes = introspection.classMap();

    QString min = completion.base;
    QString max = incrementedString(min);

    ClassMap::const_iterator matchStart, matchEnd;
    matchStart = classes.lower_bound(min);
    matchEnd = classes.lower_bound(max);
    if (matchStart == matchEnd) {
        qDebug() << "Completion: no class matches:" << completion.base;
        return NULL;
    }

    CompletionMenu * menu = new CompletionMenu(editor);

    for (ClassMap::const_iterator it = matchStart; it != matchEnd; ++it) {
        Class *klass = it->second.data();
        menu->addItem( new QStandardItem(klass->name) );
    }

    menu->adapt();

    return menu;
}

CompletionMenu * AutoCompleter::menuForClassMethodCompletion(CompletionDescription const & completion,
                                                             ScCodeEditor * editor)
{
    using namespace ScLanguage;

    const Class *klass = NULL;

    switch (completion.tokenType) {
    case Token::Float:
    case Token::RadixFloat:
    case Token::HexInt:
        // Only show completion if at least 1 character after dot
        if(!completion.base.contains(".") && completion.text.isEmpty())
            return NULL;
    default:;
    }

    klass = classForToken(completion.tokenType, completion.base);

    if (klass == NULL) {
        qDebug() << "Autocompletion not implemented for" << completion.base;
        return NULL;
    }

    QMap<QString, const Method*> methodMap; // for quick lookup
    QList<const Method*> methodList; // to keep order by class hierarchy
    do {
        foreach (const Method * method, klass->methods)
        {
            QString methodName = method->name.get();

            // Operators are also methods, but are not valid in
            // a method call syntax, so filter them out.
            Q_ASSERT(!methodName.isEmpty());
            if (!methodName[0].isLetter())
                continue;

            if (methodMap.value(methodName) != 0)
                continue;

            methodMap.insert(methodName, method);
            methodList.append(method);
        }
        klass = klass->superClass;
    } while (klass);

    CompletionMenu * menu = new CompletionMenu(editor);
    menu->setCompletionRole(CompletionMenu::CompletionRole);

    foreach(const Method *method, methodList) {
        QString methodName = method->name.get();
        QString detail(" [ %1 ]");

        QStandardItem *item = new QStandardItem();
        item->setText( methodName + detail.arg(method->ownerClass->name) );
        item->setData( QVariant::fromValue(method), CompletionMenu::MethodRole );
        item->setData( methodName, CompletionMenu::CompletionRole );
        menu->addItem(item);
    }

    menu->adapt();

    return menu;
}

CompletionMenu * AutoCompleter::menuForMethodCompletion(CompletionDescription const & completion,
                                                        ScCodeEditor * editor)
{
    using namespace ScLanguage;
    const Introspection & introspection = Main::scProcess()->introspection();

    const MethodMap & methods = introspection.methodMap();

    QString min = completion.base;
    QString max = incrementedString(min);

    MethodMap::const_iterator matchStart, matchEnd;
    matchStart = methods.lower_bound(min);
    matchEnd = methods.lower_bound(max);
    if (matchStart == matchEnd) {
        qDebug() << "Completion: no method matches:" << completion.base;
        return NULL;
    }

    CompletionMenu *menu = new CompletionMenu(editor);
    menu->setCompletionRole(CompletionMenu::CompletionRole);

    for (MethodMap::const_iterator it = matchStart; it != matchEnd; ) {
        const Method *method = it->second.data();

        std::pair<MethodMap::const_iterator, MethodMap::const_iterator> range
            = methods.equal_range(it->first);

        int count = std::distance(range.first, range.second);

        QStandardItem *item = new QStandardItem();

        QString methodName = method->name.get();
        QString detail(" [ %1 ]");
        if (count == 1) {
            item->setText( methodName + detail.arg(method->ownerClass->name) );
            item->setData( QVariant::fromValue(method), CompletionMenu::MethodRole );
        } else
            item->setText(methodName + detail.arg(count));

        item->setData(methodName, CompletionMenu::CompletionRole);

        menu->addItem(item);

        it = range.second;
    }

    menu->adapt();

    return menu;
}

const ScLanguage::Class * AutoCompleter::classForToken( Token::Type tokenType, const QString & tokenString )
{
    using namespace ScLanguage;
    const Introspection & introspection = Main::scProcess()->introspection();

    switch (tokenType) {
    case Token::Class: {
        const Class * klass = introspection.findClass(tokenString);
        if (klass) klass = klass->metaClass;
        return klass;
    }

    case Token::Float:
    case Token::RadixFloat:
    case Token::HexInt:
        if (tokenString.contains(".")) // else it is an int
            return introspection.findClass("Float");
        else
            return introspection.findClass("Integer");

    case Token::Char:
        return introspection.findClass("Char");

    case Token::StringMark:
        return introspection.findClass("String");

    case Token::Symbol:
    case Token::SymbolMark:
        return introspection.findClass("Symbol");

    default:
        ;
    }

    if (tokenString == QStringLiteral("true"))
        return introspection.findClass("True");

    if (tokenString == QStringLiteral("false"))
        return introspection.findClass("False");

    if (tokenString == QStringLiteral("nil"))
        return introspection.findClass("Nil");

    if (tokenString == QStringLiteral("thisProcess"))
        return introspection.findClass("Main");

    if (tokenString == QStringLiteral("thisFunction"))
        return introspection.findClass("Function");

    if (tokenString == QStringLiteral("thisMethod"))
        return introspection.findClass("Method");

    if (tokenString == QStringLiteral("thisFunctionDef"))
        return introspection.findClass("FunctionDef");

    if (tokenString == QStringLiteral("thisThread"))
        return introspection.findClass("Thread");

    if (tokenString == QStringLiteral("currentEnvironment"))
        return introspection.findClass("Environment");

    if (tokenString == QStringLiteral("topEnvironment"))
        return introspection.findClass("Environment");

    if (tokenString == QStringLiteral("inf"))
        return introspection.findClass("Float");

    return NULL;
}

void AutoCompleter::updateCompletionMenu(bool forceShow)
{
    Q_ASSERT(mCompletion.on && !mCompletion.menu.isNull());

    CompletionMenu *menu = mCompletion.menu;

    if (!mCompletion.text.isEmpty()) {
        QString pattern = mCompletion.text;
        pattern.prepend("^");
        menu->model()->setFilterRegExp(pattern);
    } else
        menu->model()->setFilterRegExp(QString());

    if (menu->model()->hasChildren()) {
        menu->view()->setCurrentIndex( menu->model()->index(0,0) );
        if (forceShow || menu->currentText() != mCompletion.text) {
            if (!menu->isVisible())
                menu->setTargetRect( globalCursorRect(mCompletion.pos).adjusted(0,-5,0,5) );
            // The Show event will adjust position.
            menu->show();
        } else
            menu->hide();
    } else
        menu->hide();

    if (mCompletion.type == ClassCompletion &&
            Main::settings()->value("IDE/editor/showAutocompleteHelp").toBool())
        updateCompletionMenuInfo();
}

void AutoCompleter::onCompletionMenuFinished( int result )
{
    qDebug("Completion: menu finished");

    if (!mCompletion.on)
        return;

    if (result) {
        QString text = mCompletion.menu->currentText();

        if (!text.isEmpty()) {
            quitCompletion("done");

            QTextCursor cursor( mEditor->textCursor() );
            cursor.setPosition( mCompletion.pos );
            cursor.setPosition( mCompletion.pos + mCompletion.len, QTextCursor::KeepAnchor );
            cursor.insertText(text);

            return;
        }
    }

    // Do not cancel completion whenever menu hidden.
    // It could be hidden because of current filter yielding 0 results.

    //quitCompletion("cancelled");
}

void AutoCompleter::updateCompletionMenuInfo()
{
    DocNode *node = parseHelpClass(findHelpClass(mCompletion.menu->currentText()));
    if (!node) {
        mCompletion.menu->addInfo(QString());
        return;
    }

    QString examples = parseClassElement(node, "EXAMPLES");
    if (!examples.isEmpty())
        examples.prepend("<h4>Examples</h4>");
// MSVStudio 2013 does not concatenate multiple QStringliterals ("""") properly
// see http://blog.qt.io/blog/2014/06/13/qt-weekly-13-qstringliteral/
    QString infos = QStringLiteral("<h4>%1</h4>%2%3<p><a href=\"%4\">go to help</a>")
                    .arg(parseClassElement(node, "SUMMARY"))
                    .arg(parseClassElement(node, "DESCRIPTION"))
                    .arg(examples)
                    .arg(mCompletion.menu->currentText());
    mCompletion.menu->addInfo(infos);
    doc_node_free_tree(node);
}

void AutoCompleter::triggerMethodCallAid( bool explicitTrigger )
{
    using namespace ScLanguage;

    if (!mMethodCall.menu.isNull()) {
        qDebug("Method call: disambiguation menu already shown. Aborting.");
        return;
    }

    QTextCursor cursor( mEditor->textCursor() );

    // Find the first bracket that defines a method call
    TokenIterator tokenIt;
    TokenIterator bracketIt = TokenIterator::leftOf( cursor.block(), cursor.positionInBlock() );
    while (true)
    {
        bracketIt = ScCodeEditor::previousOpeningBracket(bracketIt);
        if (!bracketIt.isValid())
            return;

        if (bracketIt->character == '(') {
            tokenIt = bracketIt.previous();
            Token::Type tokenType = tokenIt.type();

            if ( tokenIt.block() == bracketIt.block() &&
                 ( tokenType == Token::Name ||
                   tokenType == Token::Class ) )
                break;
        }

        if (!explicitTrigger)
            return;

        --bracketIt;
    }

    int bracketPos = bracketIt.position();

    // Compare against stack
    if ( !mMethodCall.stack.isEmpty() && mMethodCall.stack.top().position == bracketPos )
    {
        // A matching call is already on stack
        qDebug("Method call: trigger -> call already on stack");

        // If triggered explicitly, then either retrigger disambiguation (if needed),
        // or unsuppress it.
        if (explicitTrigger && !mMethodCall.stack.top().method) {
            qDebug("Method call: forced re-trigger, popping current call.");
            mMethodCall.stack.pop();
            hideMethodCall();
        } else {
            if (explicitTrigger) {
                mMethodCall.stack.top().suppressed = false;
                updateMethodCall(cursor.position());
            }
            // Else, method call popup has been updated by updateMethodCall()
            // called on cursor change, before this function.
            return;
        }
    }

    QString methodName;
    bool functionalNotation = false;
    const Class *receiverClass = NULL;
    Token::Type tokenType = tokenIt.type();

    Q_ASSERT( tokenType == Token::Name || tokenType == Token::Class );

    if (tokenType == Token::Name) {
        methodName = tokenText(tokenIt);
        --tokenIt;
        if (tokenIt.isValid() && tokenIt.character() == '.')
            --tokenIt;
        else
            functionalNotation = true;
    }
    else
        methodName = "new";

    if (!functionalNotation && tokenIt.isValid())
        receiverClass = classForToken( tokenIt->type, tokenText(tokenIt) );

    // Ok, this is a valid method call, push on stack

    qDebug("Method call: found call: %s:%s",
           receiverClass ? qPrintable(receiverClass->name.get()) : "",
           methodName.toStdString().c_str());

    qDebug("Method call: new call");
    MethodCall call;
    call.position = bracketPos;
    call.functionalNotation = functionalNotation;
    pushMethodCall(call);

    // Obtain method data, either by inferrence or by user-disambiguation via a menu

    const Method *method = 0;

    if (receiverClass)
    {
        const Class *klass = receiverClass;
        do {
            foreach (const Method * m, klass->methods)
            {
                if (m->name == methodName) {
                    method = m;
                    break;
                }
            }
            if (method) break;
            klass = klass->superClass;
        } while (klass);
    }
    else {
        method = disambiguateMethod( methodName, bracketPos );
    }

    // Finally, show the aid for the method

    if (method) {
        Q_ASSERT(!mMethodCall.stack.isEmpty());
        mMethodCall.stack.top().method = method;
        updateMethodCall( mEditor->textCursor().position() );
    }
}

const ScLanguage::Method *AutoCompleter::disambiguateMethod
( const QString & methodName, int cursorPos )
{
    Q_ASSERT(mMethodCall.menu.isNull());

    using namespace ScLanguage;
    using std::pair;

    const Introspection & introspection = Main::scProcess()->introspection();
    const MethodMap & methods = introspection.methodMap();

    pair<MethodMap::const_iterator, MethodMap::const_iterator> match =
        methods.equal_range(methodName);

    const Method *method = 0;

    if (match.first == match.second) {
        qDebug() << "MethodCall: no method matches:" << methodName;
        method = 0;
    }
    else if (std::distance(match.first, match.second) == 1)
        method = match.first->second.data();
    else {
        QPointer<CompletionMenu> menu = new CompletionMenu(mEditor);
        mMethodCall.menu = menu;

        for (MethodMap::const_iterator it = match.first; it != match.second; ++it)
        {
            const Method *method = it->second.data();
            QStandardItem *item = new QStandardItem();
            item->setText(method->name + " (" + method->ownerClass->name + ')');
            item->setData( QVariant::fromValue(method), CompletionMenu::MethodRole );
            menu->addItem(item);
        }

        menu->adapt();

        QRect popupTargetRect = globalCursorRect( cursorPos ).adjusted(0,-5,0,5);

        if ( static_cast<PopUpWidget*>(menu)->exec(popupTargetRect) )
            method = menu->currentMethod();

        delete menu;
    }

    return method;
}

void AutoCompleter::updateMethodCall( int cursorPos )
{
    int i = mMethodCall.stack.count();
    while (i--)
    {
        MethodCall & call = mMethodCall.stack[i];
        int argNum = -1;
        TokenIterator argNameToken;

        if (!testMethodCall(call, cursorPos, argNum, argNameToken)) {
            qDebug("Method call: popping.");
            Q_ASSERT(i == mMethodCall.stack.count() - 1);
            mMethodCall.stack.pop();
            continue;
        }

        if (call.suppressed) {
            qDebug("Method call: suppressed, not showing anything");
            break;
        }

        if (!call.method || !call.method->arguments.count()) {
            qDebug("Method call: no info to show. skipping.");
            continue;
        }

        if (argNameToken.isValid()) {
            QString argName = tokenText(argNameToken);
            argName.chop(1);
            for (int idx = 0; idx < call.method->arguments.count(); ++idx) {
                if (call.method->arguments[idx].name == argName) {
                    argNum = idx;
                    if (call.functionalNotation)
                        ++argNum;
                    break;
                }
            }
        }
        qDebug("Method call: found current call: %s(%i)",
            call.method->name.get().toStdString().c_str(), argNum);
        showMethodCall(call, argNum);
        return;
    }

    hideMethodCall();
}

void AutoCompleter::pushMethodCall( const MethodCall & call )
{
    qDebug("Method Call: pushing on stack.");
    Q_ASSERT( mMethodCall.stack.isEmpty()
        || mMethodCall.stack.last().position < call.position );

    mMethodCall.stack.push(call);
}

void AutoCompleter::showMethodCall( const MethodCall & call, int arg )
{
    if (mMethodCall.widget.isNull())
        mMethodCall.widget = new MethodCallWidget(mEditor);

    MethodCallWidget *w = mMethodCall.widget;

    w->showMethod( call, arg, globalCursorRect(call.position).adjusted(0, -7, 0, 5) );
}

void AutoCompleter::hideMethodCall()
{
    delete mMethodCall.widget;
}

bool AutoCompleter::trySwitchMethodCallArgument(bool backwards)
{
    // FIXME: Only cycle through argument names that have not been entered already

    using namespace ScLanguage;

    QTextCursor cursor( mEditor->textCursor() );
    if (cursor.hasSelection())
        return false;

    if (mMethodCall.stack.isEmpty()) {
        qDebug("Insert arg name: empty stack");
        return false;
    }

    MethodCall & call = mMethodCall.stack.top();
    if (!call.method || !call.method->arguments.count()) {
        qDebug("Insert arg name: no method, or method has no args");
        return false;
    }

    int cursorPos = cursor.position();
    int argNum = -1;
    TokenIterator argNameToken;

    static const bool strict = true;
    bool callValid = testMethodCall( call, cursorPos, argNum, argNameToken, strict );
    if (!callValid) {
        qDebug("Insert arg name: call invalid");
        return false;
    }

    bool cursorAtArgName = argNum == -1;

    if (argNameToken.isValid()) {
        qDebug("Insert arg name: have a reference arg name");
        QString argName = tokenText(argNameToken);
        argName.chop(1);
        for (int idx = 0; idx < call.method->arguments.count(); ++idx) {
            if (call.method->arguments[idx].name == argName) {
                argNum = idx;
                break;
            }
        }
        // only increment/decrement if a reference name exists
        if (backwards)
            --argNum;
        else
            ++argNum;
    }

    // limit / wrap
    if (argNum < 0)
        argNum = call.method->arguments.count() - 1;
    else if (argNum >= call.method->arguments.count())
        argNum = 0;

    QString text = call.method->arguments[argNum].name;
    text.append(":");

    // insert argument name
    if (argNameToken.isValid() && cursorAtArgName) {
        int pos = argNameToken.position();
        cursor.setPosition(pos);
        cursor.setPosition(pos + argNameToken->length, QTextCursor::KeepAnchor);
    }
    cursor.insertText(text);

    return true;
}

bool AutoCompleter::testMethodCall( const MethodCall &call, int cursorPos,
                                    int &outArgNum, TokenIterator &outArgNameToken,
                                    bool strict )
{
    // The 'strict' argument denotes whether the test passes if token before cursor is
    // not a comma or an argument name - i.e. the user is typing the value of an argument

    if (call.position >= cursorPos) {
        qDebug("Method call: call right of cursor.");
        return false;
    }

    QTextBlock block( document()->findBlock( call.position ) );
    TokenIterator token = TokenIterator::rightOf(block, call.position - block.position());
    if (!token.isValid())
        qWarning("Method call: call stack out of sync!");
    Q_ASSERT(token.isValid());

    int argNum = 0;
    int level = 1;
    TokenIterator argNameToken;
    bool strictlyValid = true;

    ++token;
    while( level > 0 && token.isValid() && token.position() < cursorPos )
    {
        strictlyValid = false;

        char chr = token.character();
        Token::Type type = token->type;
        if (level == 1) {
            if (type == Token::SymbolArg) {
                argNameToken = token;
                argNum = -1; // denote argument name
                strictlyValid = true;
            }
            else if (chr == ',') {
                if (argNum >= 0)
                    ++argNum;
                else
                    argNum = -2; // denote comma after argument name
                strictlyValid = true;
            }
        }

        if (type == Token::OpeningBracket)
            ++level;
        else if (type == Token::ClosingBracket)
            --level;

        ++token;
    }

    if (level <= 0) {
        qDebug("Method call: call left of cursor.");
        return false;
    }

    outArgNameToken = argNameToken;
    outArgNum = argNum;

    return (strictlyValid || !strict);
}

void AutoCompleter::clearMethodCallStack()
{
    mMethodCall.stack.clear();
    hideMethodCall();
}

void AutoCompleter::hideWidgets()
{
    if (mCompletion.menu)
        mCompletion.menu->reject();
    if (mMethodCall.menu)
        mMethodCall.menu->reject();
    if (mMethodCall.widget)
        mMethodCall.widget->hide();
}

QString AutoCompleter::tokenText( TokenIterator & it )
{
    if (!it.isValid())
        return QString();

    int pos = it.position();
    QTextCursor cursor(document());
    cursor.setPosition(pos);
    cursor.setPosition(pos + it->length, QTextCursor::KeepAnchor);
    return cursor.selectedText();
}

QRect AutoCompleter::globalCursorRect( int cursorPosition )
{
    QTextCursor cursor(document());
    cursor.setPosition(cursorPosition);
    QRect r = mEditor->cursorRect(cursor);
    r.moveTopLeft( mEditor->viewport()->mapToGlobal( r.topLeft() ) );
    return r;
}

QString AutoCompleter::findHelpClass(QString klass)
{
    QString file = standardDirectory(ScResourceDir)
                                    .append("/HelpSource/Classes/")
                                    .append(klass).append(".schelp");
    if (QFile::exists(file))
        return file;

    return QString();
}

DocNode * AutoCompleter::parseHelpClass(QString file)
{
    if (file.isEmpty())
        return NULL;

    // note, toStdString() converts to UTF-8
    return scdoc_parse_file(file.toStdString(), 0);
}

QString AutoCompleter::parseClassElement(DocNode *node, QString element)
{
    if (QString(node->id) == element) {
        QString str;
        parseClassNode(node, &str);
        return str;
    }

    for (int i = 0; i < node->n_childs; i++) {
        QString ret = parseClassElement(node->children[i], element);
        if (!ret.isEmpty())
            return ret;
    }

    return QString();
}

void AutoCompleter::parseClassNode(DocNode *node, QString *str)
{
    QString id = node->id;

    if (id == "NOTE")
        str->append("<br><br>Note:<br>");

    if (node->text) {
        if (id == "LINK") {
            QStringList locations = QString(node->text).split('/').last().split('#');

            /* if empty, the link is on the same page. No HTML link */
            if (locations.first().isEmpty())
                str->append(QStringLiteral(" %1 ").arg(locations.first()));
            else
                str->append(QStringLiteral("<a href=\"%1\">%2</a>").arg(locations.first())
                                                            .arg(locations.last()));
        } else if (id == "CODE")  {
            str->append(QStringLiteral("<code>%1</code>").arg(node->text));
        } else if (id == "CODEBLOCK")  {
            str->append(QStringLiteral("<pre><code>%1</code></pre>").arg(node->text));
        } else {
            str->append(node->text);
        }
    }

    for (int i = 0; i < node->n_childs; i++)
        parseClassNode(node->children[i], str);
}

void AutoCompleter::gotoHelp(QString symbol)
{
    HelpBrowserDocklet *helpDock = MainWindow::instance()->helpBrowserDocklet();
    helpDock->browser()->gotoHelpFor(symbol);
    helpDock->focus();
}

} // namespace ScIDE

#undef QT_NO_DEBUG_OUTPUT
