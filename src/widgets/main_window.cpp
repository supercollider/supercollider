#include "main_window.hpp"
#include "post_window.hpp"

#include <QAction>
#include <QMenu>
#include <QMenuBar>

namespace ScIDE {

MainWindow::MainWindow()
{
    _actions.resize(ActionCount);

    QAction *act;

    _actions[DocNew] = act = new QAction(tr("&New"), this);
    act->setShortcuts(QKeySequence::New);
    act->setStatusTip(tr("Create a new file"));

    _actions[DocOpen] = act = new QAction(tr("&Open..."), this);
    act->setShortcuts(QKeySequence::Open);
    act->setStatusTip(tr("Open an existing file"));

    _actions[Quit] = act = new QAction(tr("&Quit..."), this);
    act->setShortcuts(QKeySequence::Quit);
    act->setStatusTip(tr("Quit SuperCollider IDE"));

    QMenu *menu = new QMenu(tr("&File"), this);
    Q_FOREACH( QAction *a, _actions )
        if(a) menu->addAction(a);

    menuBar()->addMenu(menu);

    PostDock * postDock = new PostDock(this);
    addDockWidget(Qt::RightDockWidgetArea, postDock);
}

QAction *MainWindow::action( ActionRole role )
{
    Q_ASSERT( role < ActionCount );
    return _actions[role];
}

} // namespace ScIDE
