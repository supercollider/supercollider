/*
    SuperCollider Qt IDE
    Copyright (c) 2010-2012 Jakob Leben
    Copyright (c) 2012 Tim Blechmann
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

#include "cmd_line.hpp"
#include "main_window.hpp"
#include "util/gui_utilities.hpp"
#include "../core/main.hpp"
#include "../core/settings/manager.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QKeyEvent>

namespace ScIDE {

QString CmdLineEdit::symbolUnderCursor() {
    if (hasSelectedText())
        return selectedText();
    else {
        int position = cursorPosition();
        return tokenInStringAt(position, text());
    }
}

bool CmdLineEdit::openDocumentation() { return Main::openDocumentation(symbolUnderCursor()); }

void CmdLineEdit::openDefinition() { return Main::openDefinition(symbolUnderCursor(), MainWindow::instance()); }

void CmdLineEdit::openCommandLine() { return Main::openCommandLine(symbolUnderCursor()); }

void CmdLineEdit::findReferences() { return Main::findReferences(symbolUnderCursor(), MainWindow::instance()); }


CmdLine::CmdLine(const QString& text, int maxHist): curHistory(-1), maxHistory(qMax(1, maxHist)) {
    QLabel* lbl = new QLabel(text);

    expr = new CmdLineEdit;

    QHBoxLayout* l = new QHBoxLayout;
    l->setContentsMargins(0, 0, 0, 0);
    l->addWidget(lbl);
    l->addWidget(expr);
    setLayout(l);

    expr->installEventFilter(this);
    setFocusProxy(expr);

    applySettings(Main::settings());
}

void CmdLine::applySettings(Settings::Manager* settings) {
    QFont codeFont = settings->codeFont();
    expr->setFont(codeFont);
}

void CmdLine::setText(const QString& text) { expr->setText(text); }

bool CmdLine::eventFilter(QObject*, QEvent* e) {
    int type = e->type();
    if (type != QEvent::KeyPress)
        return false;

    QKeyEvent* ke = static_cast<QKeyEvent*>(e);

    switch (ke->key()) {
    case Qt::Key_Return:
    case Qt::Key_Enter:

        if (expr->text().isEmpty())
            return true;

        emit invoked(expr->text(), false);
        if (history.count() == 0 || history[0] != expr->text()) {
            if (history.count() >= maxHistory)
                history.removeAt(history.count() - 1);
            history.prepend(expr->text());
        }
        curHistory = -1;
        expr->clear();
        return true;

    case Qt::Key_Up:
        if (curHistory < history.count() - 1) {
            expr->blockSignals(true);
            expr->setText(history[++curHistory]);
            expr->blockSignals(false);
        }
        return true;

    case Qt::Key_Down:
        if (curHistory > -1) {
            --curHistory;
            expr->blockSignals(true);
            if (curHistory == -1)
                expr->clear();
            else
                expr->setText(history[curHistory]);
            expr->blockSignals(false);
        }
        return true;

    default:
        return false;
    }
}

} // namespace ScIDE
