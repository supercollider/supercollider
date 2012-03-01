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

#include "widgets/main_window.hpp"
#include "main.hpp"

#include <QApplication>
#include <QAction>

using namespace ScIDE;

int main( int argc, char *argv[] )
{
    Main * main = Main::instance();

    QApplication app(argc, argv);

    MainWindow *win = new MainWindow(main);

    QObject::connect( win->action(MainWindow::Quit), SIGNAL(triggered()),
                      &app, SLOT(quit()) );

    win->showMaximized();

    return app.exec();
}

Main::Main(void) :
    mDocManager( new DocumentManager(this) ),
    mSCProcess( new SCProcess(this) )
{}
