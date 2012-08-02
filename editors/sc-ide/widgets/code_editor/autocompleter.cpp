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

#include "autocompleter.hpp"
#include "tokens.hpp"
#include "editor.hpp"
#include "../util/popup_widget.hpp"
#include "../../core/sc_process.hpp"
#include "../../core/main.hpp"

#include "yaml-cpp/node.h"
#include "yaml-cpp/parser.h"

#include <QDebug>
#include <QLabel>
#include <QListView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QHBoxLayout>
#include <QApplication>

namespace ScIDE {

static bool tokenMaybeName( Token::Type type )
{
    return (type == Token::Name || type == Token::Keyword || type == Token::Builtin);
}

static AutoCompleter::Method parseMethod( const YAML::Node & node )
{
    Q_ASSERT(node.Type() == YAML::NodeType::Sequence);
    Q_ASSERT(node.size() >= 2);

    assert(node[0].Type() == YAML::NodeType::Scalar);
    assert(node[1].Type() == YAML::NodeType::Scalar);

    AutoCompleter::Method m;
    m.className = node[0].to<std::string>().c_str();
    m.methodName = node[1].to<std::string>().c_str();
    YAML::Iterator it = node.begin();
    ++it; ++it;
    while (it != node.end())
    {
        // get arg name
        assert(it->Type() == YAML::NodeType::Scalar);
        m.argNames << it->to<std::string>().c_str();
        // get arg default value
        ++it;
        if (it == node.end())
            break;
        if(it->Type() == YAML::NodeType::Scalar)
            m.argDefaults << it->to<std::string>().c_str();
        else
            m.argDefaults << QString();
        // next arg
        ++it;
    }

    return m;
}

static QString methodSignature( const AutoCompleter::Method & method )
{
    QString text = method.methodName;
    text += "(";
    int argc = method.argNames.count();
    for (int i = 0; i < argc; ++i) {
        text += method.argNames[i];
        QString val = method.argDefaults[i];
        if (!val.isEmpty())
            text += " = " + val;
        if (i != argc - 1)
            text += ", ";
    }
    text +=")";
    return text;
}

class CompletionMenu : public PopUpWidget
{
public:
    enum DataRole {
        CompletionRole = Qt::UserRole,
        MethodRole
    };

    CompletionMenu( QWidget * parent = 0 ):
        PopUpWidget(parent),
        mCompletionRole( Qt::DisplayRole )
    {
        mModel = new QStandardItemModel(this);
        mFilterModel = new QSortFilterProxyModel(this);
        mFilterModel->setSourceModel(mModel);

        mListView = new QListView();
        mListView->setModel(mFilterModel);
        mListView->setFrameShape(QFrame::NoFrame);

        QHBoxLayout *layout = new QHBoxLayout(this);
        layout->addWidget(mListView);
        layout->setContentsMargins(1,1,1,1);

        connect(mListView, SIGNAL(clicked(QModelIndex)), this, SLOT(accept()));
        connect(mListView, SIGNAL(activated(QModelIndex)), this, SLOT(accept()));

        mListView->setFocus(Qt::OtherFocusReason);

        resize(200, 200);
    }

    void addItem( QStandardItem * item )
    {
        mModel->appendRow( item );
    }

    void setCompletionRole( int role )
    {
        mFilterModel->setFilterRole(role);
        mFilterModel->setSortRole(role);
        mCompletionRole = role;
    }

    QString currentText()
    {
        QStandardItem *item =
            mModel->itemFromIndex (
                mFilterModel->mapToSource (
                    mListView->currentIndex()));
        if (item)
            return item->data(mCompletionRole).toString();

        return QString();
    }

    AutoCompleter::Method currentMethod()
    {
        QStandardItem *item =
            mModel->itemFromIndex (
                mFilterModel->mapToSource (
                    mListView->currentIndex()));
        if (item)
            return item->data(MethodRole).value<AutoCompleter::Method>();

        return AutoCompleter::Method();
    }

    QString exec( const QPoint & pos )
    {
        QString result;
        QPointer<CompletionMenu> self = this;
        if (PopUpWidget::exec(pos)) {
            if (!self.isNull())
                result = currentText();
        }
        return result;
    }

    QSortFilterProxyModel *model() { return mFilterModel; }

    QListView *view() { return mListView; }

protected:
    virtual bool eventFilter( QObject * obj, QEvent * ev )
    {
        if (isVisible() && obj == parentWidget() && ev->type() == QEvent::KeyPress)
        {
            QKeyEvent *kev = static_cast<QKeyEvent*>(ev);
            switch(kev->key())
            {
            case Qt::Key_Up:
            case Qt::Key_Down:
            case Qt::Key_PageUp:
            case Qt::Key_PageDown:
            case Qt::Key_Return:
            case Qt::Key_Enter:
                QApplication::sendEvent( mListView, ev );
                return true;
            }
        }

        return PopUpWidget::eventFilter(obj, ev);
    }

private:
    QListView *mListView;
    QStandardItemModel *mModel;
    QSortFilterProxyModel *mFilterModel;
    int mCompletionRole;
};

class MethodCallWidget : public QWidget
{
public:
    MethodCallWidget( QWidget * parent = 0 ):
        QWidget( parent, Qt::ToolTip )
    {
        QPalette p;
        p.setColor( QPalette::Window, p.color(QPalette::ToolTipBase) );
        p.setColor( QPalette::WindowText, p.color(QPalette::ToolTipText) );
        setPalette(p);

        mLabel = new QLabel();

        QHBoxLayout *box = new QHBoxLayout;
        box->setContentsMargins(5,2,5,2);
        box->addWidget(mLabel);
        setLayout(box);
    }

    void showMethod( const AutoCompleter::Method & method, int arg )
    {
        QString text = method.methodName;
        text += " (";
        int argc = method.argNames.count();
        for (int i = 0; i < argc; ++i) {
            if (i == arg)
                text += "<b>" + method.argNames[i] + "</b>";
            else
                text += method.argNames[i];
            QString val = method.argDefaults[i];
            if (!val.isEmpty())
                text += " = " + val;
            if (i != argc - 1)
                text += ", ";
        }
        text +=")";
        mLabel->setText(text);
    }

private:
    QLabel *mLabel;
};

AutoCompleter::AutoCompleter( CodeEditor *editor ):
    QObject(editor),
    mEditor(editor),
    mCompletionRequest( new ScRequest(Main::instance()->scProcess(), this) ),
    mMethodCallRequest( new ScRequest(Main::instance()->scProcess(), this) )
{
    mCompletion.on = false;
    mMethodCall.pos = -1;

    connect(editor, SIGNAL(cursorPositionChanged()),
            this, SLOT(onCursorChanged()));
    connect(mCompletionRequest, SIGNAL(response(QString,QString)),
            this, SLOT(onCompletionResponse(QString,QString)));
    connect(mMethodCallRequest, SIGNAL(response(QString,QString)),
            this, SLOT(onMethodCallResponse(QString,QString)));
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
    int key = e->key();
    switch (e->key())
    {
    case Qt::Key_ParenLeft:
    case Qt::Key_Comma:
        startMethodCall();
        break;
    case Qt::Key_Backspace:
    case Qt::Key_Delete:
        return;
    default:
        qDebug(">>> key");
        if (!e->text().isEmpty() && !mCompletion.on)
            startCompletion();
    }
#if 0
    if ((key >= Qt::Key_0 && key <= Qt::Key_9) ||
        (key >= Qt::Key_A && key <= Qt::Key_Z) ||
        key == Qt::Key_Period)
#endif
}

void AutoCompleter::onContentsChange( int pos, int removed, int added )
{
    qDebug(">>> contentsChange");

    while (!mMethodCall.stack.isEmpty())
    {
        MethodCall & call = mMethodCall.stack.top();
        if (pos > call.position)
            break;
        else {
            qDebug("Method call: change before method call. popping.");
            mMethodCall.stack.pop();
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
                updateCompletionMenu();
        }
    }
}

void AutoCompleter::onCursorChanged()
{
    qDebug(">>> cursorChanged");
    int cursorPos = mEditor->textCursor().position();

    // completion
    if (mCompletion.on) {
        if (cursorPos < mCompletion.pos ||
            cursorPos > mCompletion.pos + mCompletion.len)
        {
            quitCompletion("out of bounds");
        }
    }

    if (mMethodCall.pos != -1) {
        qDebug("Method call: cancelling sclang request");
        mMethodCall.pos = -1;
        mMethodCallRequest->cancel();
    }

    updateMethodCall(cursorPos);
}

void AutoCompleter::startCompletion()
{
    if (mCompletion.on) {
        qWarning("AutoCompleter::startCompletion(): completion already started!");
        return;
    }

    QTextCursor cursor( mEditor->textCursor() );
    int cursorPos = cursor.positionInBlock();
    QTextBlock block( cursor.block() );
    TokenIterator it( block, cursorPos - 1 );

    if (!it.isValid())
        return;

    const Token & token = *it;
    QString contextText, command;

    if (token.type == Token::Class)
    {
        if (token.length < 3)
            return;
        mCompletion.type = ClassCompletion;
        mCompletion.pos = it.position();
        mCompletion.len = it->length;
        mCompletion.text = tokenText(it);
        mCompletion.contextPos = mCompletion.pos + 3;
        contextText = mCompletion.text;
        contextText.truncate(3);
        command = "completeClass";
    }
    else {
        // Parse method call

        TokenIterator cit, dit, mit;

        if (token.character == '.')
        {
            dit = it;
            --it;
            if (it.type() == Token::Class)
                cit = it;
            else
                return;
            TokenIterator it = dit.next();
            if (tokenMaybeName(it.type())
                && it.block() == dit.block()
                && it->position == dit->position + 1)
                    mit = it;
        }
        else if (tokenMaybeName(token.type))
        {
            mit = it;
            --it;
            if (it.isValid() && it->character == '.')
                dit = it;
            else
                return;
            --it;
            if (it.type() == Token::Class)
                cit = it;
        }
        else
            return;

        if (!cit.isValid() && mit->length < 3)
            return;

        if (mit.isValid()) {
            mCompletion.pos = mit.position();
            mCompletion.len = mit->length;
            mCompletion.text = tokenText(mit);
        }
        else {
            mCompletion.pos = dit.position() + 1;
            mCompletion.len = 0;
            mCompletion.text.clear();
        }

        if (cit.isValid()) {
            mCompletion.contextPos = mCompletion.pos;
            contextText = tokenText(cit);
            command = "completeClassMethod";
            mCompletion.type = ClassMethodCompletion;
        }
        else {
            mCompletion.contextPos = mCompletion.pos + 3;
            contextText = tokenText(mit);
            contextText.truncate(3);
            command = "completeMethod";
            mCompletion.type = MethodCompletion;
        }
    }

    mCompletion.on = true;
    qDebug() << "Completion: ON";
    qDebug() << "Completion: sending request:" << command << contextText;
    mCompletionRequest->send( command, contextText );
}

void AutoCompleter::quitCompletion( const QString & reason )
{
    Q_ASSERT(mCompletion.on);

    qDebug() << QString("Completion: OFF (%1)").arg(reason);

    mCompletionRequest->cancel();

    if (mCompletion.menu) {
        mCompletion.menu->hide();
        mCompletion.menu->deleteLater();
        mCompletion.menu = 0;
    }

    mCompletion.on = false;
}

void AutoCompleter::onCompletionResponse( const QString & cmd, const QString & data )
{
    qDebug(">>> completion response");

    if ( cmd != "completeClass" &&
         cmd != "completeMethod" &&
         cmd != "completeClassMethod")
        return;

    if (!mCompletion.on) {
        qWarning("Completion: sclang response while completion off!");
        return;
    }

    if (!mCompletion.menu.isNull()) {
        qWarning("Completion: can not show new menu - one already shown!");
        return;
    }

    QPointer<CompletionMenu> popup = new CompletionMenu(mEditor);

    std::stringstream stream;
    stream << data.toStdString();
    YAML::Parser parser(stream);

    YAML::Node doc;
    if(!parser.GetNextDocument(doc) || doc.Type() != YAML::NodeType::Sequence) {
        qWarning("Bad YAML data!");
        delete popup;
        return;
    }

    switch (mCompletion.type)
    {
    case ClassCompletion:
    {
        for (YAML::Iterator it = doc.begin(); it != doc.end(); ++it)
        {
            YAML::Node const & entry = *it;
            if(entry.Type() == YAML::NodeType::Scalar) {
                QString text( entry.to<std::string>().c_str() );
                popup->addItem( new QStandardItem(text) );
            }
            else
                qWarning("YAML parsing: a YAML data entry not a scalar");
        }
        break;
    }
    case ClassMethodCompletion:
    case MethodCompletion:
    {
        for (YAML::Iterator it = doc.begin(); it != doc.end(); ++it)
        {
            YAML::Node const & entry = *it;
            if (entry.Type() != YAML::NodeType::Sequence) {
                qWarning("YAML parsing: a YAML data entry not a Sequence");
                continue;
            }
            if (entry.size() < 2) {
                qWarning("YAML parsing: two few sequence elements");
                continue;
            }
            Method m = parseMethod(entry);
            QStandardItem *item = new QStandardItem();
            if (mCompletion.type == ClassMethodCompletion)
                item->setText(m.methodName);
            else {
                item->setText(m.methodName + " (" + m.className + ')');
                item->setData(m.methodName, CompletionMenu::CompletionRole);
            }
            item->setData( QVariant::fromValue<Method>(m), CompletionMenu::MethodRole );
            popup->addItem(item);
        }
        break;
    }
    default:
        Q_ASSERT(false);
    }

    if (mCompletion.type == MethodCompletion)
        popup->setCompletionRole(CompletionMenu::CompletionRole);

    mCompletion.menu = popup;

    connect(popup, SIGNAL(finished(int)), this, SLOT(onCompletionMenuFinished(int)));

    QTextCursor cursor(document());
    cursor.setPosition(mCompletion.pos);
    QPoint pos =
        mEditor->viewport()->mapToGlobal( mEditor->cursorRect(cursor).bottomLeft() )
        + QPoint(0,5);

    popup->popup(pos);

    updateCompletionMenu();
}

void AutoCompleter::updateCompletionMenu()
{
    Q_ASSERT(mCompletion.on && !mCompletion.menu.isNull());

    CompletionMenu *menu = mCompletion.menu;

    if (!mCompletion.text.isEmpty()) {
        QString pattern = mCompletion.text;
        pattern.prepend("^");
        menu->model()->setFilterRegExp(pattern);
    }
    else {
        menu->model()->setFilterRegExp(QString());
    }

    if (menu->model()->hasChildren()) {
        menu->model()->sort(0);
        menu->view()->setCurrentIndex( menu->model()->index(0,0) );
        menu->show();
    }
    else {
        menu->hide();
    }
}

void AutoCompleter::onCompletionMenuFinished( int result )
{
    qDebug("Completion: menu finished");

    if (!mCompletion.on)
        return;

    if (result) {
        QString text = mCompletion.menu->currentText();

        if (!text.isEmpty()) {
            CompletionType type = mCompletion.type;
            Method method;
            if (type == MethodCompletion || type == ClassMethodCompletion)
                method = mCompletion.menu->currentMethod();

            quitCompletion("done");

            QTextCursor cursor( mEditor->textCursor() );
            cursor.setPosition( mCompletion.pos );
            cursor.setPosition( mCompletion.pos + mCompletion.len, QTextCursor::KeepAnchor );
            cursor.insertText(text);

            if (!method.methodName.isEmpty()) {
                cursor.insertText("(");
                MethodCall call;
                call.position = cursor.position() - 1;
                call.method = method;
                pushMethodCall(call);
                showMethodCall(call, 0);
            }

            return;
        }
    }

    // Do not cancel completion whenever menu hidden.
    // It could be hidden because of current filter yielding 0 results.

    //quitCompletion("cancelled");
}

void AutoCompleter::startMethodCall()
{
    // go find the bracket that I'm currently in,
    // and count relevant commas along the way

    QTextDocument *doc = document();
    QTextCursor cursor( mEditor->textCursor() );

    int pos = cursor.position();

    QTextBlock block( doc->findBlock(pos) );
    if (!block.isValid())
        return;
    pos -= block.position();

    TokenIterator it( TokenIterator::leftOf( block, pos ) );

    int level = 1;
    int argPos = 0;

    while (it.isValid())
    {
        char chr = it->character;
        Token::Type type = it->type;
        if (chr == ',') {
            if (level == 1)
                ++argPos;
        }
        else if (type == Token::ClosingBracket)
            ++level;
        else if (type == Token::OpeningBracket)
        {
            --level;
            if (level == 0) {
                if (chr == '(')
                    break;
                else
                    return;
            }
        }
        --it;
    }

    if (!it.isValid())
        return;

    int bracketPos;
    pos = bracketPos = it.position();

    QString className, methodName;

    --it;
    if (it.isValid())
    {
        int type = it->type;
        if (type == Token::Class) {
            className = tokenText(it);
            methodName = "new";
        }
        else if (type == Token::Name) {
            methodName = tokenText(it);
            --it;
            if (it.isValid() && it->character == '.') {
                --it;
                if (it.isValid() && it->type == Token::Class)
                    className = tokenText(it);
            }
        }
    }

    if (methodName.isEmpty())
        return;

    qDebug("Method call: found call: %s.%s(%i)",
           className.toStdString().c_str(),
           methodName.toStdString().c_str(),
           argPos);

    if ( !mMethodCall.stack.isEmpty() && mMethodCall.stack.last().position == bracketPos )
    {
        qDebug("Method call: call already on stack");
        // method call popup should have been updated by updateMethodCall();
    }
    else
    {
        qDebug("Method call: new call");
        MethodCall call;
        call.position = bracketPos;
        pushMethodCall(call);

        // store position for sc response
        mMethodCall.pos = bracketPos;

        QString text = className;
        if (!text.isEmpty())
            text.append('.');
        text.append(methodName);

        mMethodCallRequest->send( "findMethod", text );
    }
}

void AutoCompleter::updateMethodCall( int cursorPos )
{
    QTextDocument *doc = document();

    int i = mMethodCall.stack.count();
    while (i--)
    {
        MethodCall & call = mMethodCall.stack[i];
        if (call.position >= cursorPos) {
            qDebug("Method call: call right of cursor. popping.");
            mMethodCall.stack.pop();
            continue;
        }

        QTextBlock block( document()->findBlock( call.position ) );
        TokenIterator token = TokenIterator::rightOf(block, call.position - block.position());
        if (!token.isValid()) {
            qWarning("Method call: call stack out of sync!");
            mMethodCall.stack.clear();
            break;
        }

        ++token;
        int arg = 0;
        int level = 1;
        while( level > 0 && token.isValid() && token.position() < cursorPos )
        {
            char chr = token.character();
            Token::Type type = token->type;
            if (chr == ',' && level == 1)
                ++arg;
            else if (type == Token::OpeningBracket)
                ++level;
            else if (type == Token::ClosingBracket)
                --level;

            ++token;
        }

        if (level > 0) {
            if (call.method.methodName.isEmpty())
                qDebug("Method call: call data incomplete. skipping.");
            else {
                qDebug("Method call: found current call: %s(%i)",
                    call.method.methodName.toStdString().c_str(), arg);
                showMethodCall(call, arg);
                return;
            }
        }
        else {
            Q_ASSERT(i == mMethodCall.stack.count() - 1);
            qDebug("Method call: call left of cursor. popping.");
            mMethodCall.stack.pop();
        }
    }

    hideMethodCall();
}

void AutoCompleter::onMethodCallResponse( const QString & cmd, const QString & data )
{
    qDebug(">>> method call response...");

    if (cmd != "findMethod" )
        return;

    static QPointer<CompletionMenu> popup;

    Q_ASSERT(popup.isNull());
    // FIXME: check that method call is still relevant, even before popup!

    std::stringstream stream;
    stream << data.toStdString();
    YAML::Parser parser(stream);

    YAML::Node doc;
    if(!parser.GetNextDocument(doc) || doc.Type() != YAML::NodeType::Sequence) {
        qWarning("YAML parsing: document not a Sequence!");
        return;
    }

    QList<Method> methods;

    for (YAML::Iterator it = doc.begin(); it != doc.end(); ++it)
    {
        YAML::Node const & entry = *it;
        if (entry.Type() != YAML::NodeType::Sequence) {
            qWarning("YAML parsing: node not a Sequence!");
            continue;
        }
        if (entry.size() < 2) {
            qWarning("YAML parsing: two few sequence elements!");
            continue;
        }
        methods.append( parseMethod(entry) );
    }

    Method method;

    if (methods.count() == 1)
        method = methods.first();
    else {
        popup = new CompletionMenu(mEditor);

        foreach( const Method & m, methods ) {
            QStandardItem *item = new QStandardItem();
            item->setText(m.methodName + " (" + m.className + ')');
            item->setData(m.methodName, CompletionMenu::CompletionRole);
            item->setData( QVariant::fromValue<Method>(m), CompletionMenu::MethodRole );
            popup->addItem(item);
        }

        QTextCursor cursor(document());
        cursor.setPosition(mMethodCall.pos);
        QPoint pos =
            mEditor->viewport()->mapToGlobal( mEditor->cursorRect(cursor).bottomLeft() )
            + QPoint(0,5);

        PopUpWidget *w = static_cast<PopUpWidget*>( popup );
        if (!w->exec(pos)) {
            delete popup;
            return;
        }

        method = popup->currentMethod();
        delete popup;
    }

    if (!method.methodName.isEmpty()
        && !mMethodCall.stack.isEmpty()
        && mMethodCall.stack.top().position == mMethodCall.pos)
    {
        mMethodCall.stack.top().method = method;
        updateMethodCall( mEditor->textCursor().position() );
    }
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
    QTextCursor cursor(document());
    cursor.setPosition(call.position);
    QPoint pos =
        mEditor->viewport()->mapToGlobal( mEditor->cursorRect(cursor).topLeft() );
    pos += QPoint(0, -20);

    if (mMethodCall.widget.isNull())
        mMethodCall.widget = new MethodCallWidget(mEditor->viewport());

    MethodCallWidget *w = mMethodCall.widget;

    w->showMethod( call.method, arg );
    w->resize(w->sizeHint());
    w->move(pos);
    w->show();
}

void AutoCompleter::hideMethodCall()
{
    delete mMethodCall.widget;
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

} // namespace ScIDE
