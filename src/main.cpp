#include "widgets/main_window.hpp"

#include <QApplication>
#include <QAction>

using namespace ScIDE;

int main( int argc, char *argv[] )
{
    QApplication app(argc, argv);

    MainWindow *win = new MainWindow();

    QObject::connect( win->action(MainWindow::Quit), SIGNAL(triggered()),
                      &app, SLOT(quit()) );

    win->show();

    return app.exec();
}
