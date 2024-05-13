#include <QDebug>

void error(const char* fmt, ...) { qWarning() << fmt; }

void post(const char* fmt, ...) { qDebug() << fmt; }
