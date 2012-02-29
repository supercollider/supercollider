#include "widgets/main_window.hpp"

#include <QApplication>

using namespace ScIDE;

int main( int argc, char *argv[] )
{
    QApplication app(argc, argv);

    MainWindow *win = new MainWindow();
    win->show();

    return app.exec();
}
