// http://www.thehelper.net/threads/guide-explanation-of-w3m-and-w3x-files.35292/

#include <QApplication>

#include "Window.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFile File("stylesheet.qss");
    File.open(QFile::ReadOnly);
    a.setStyleSheet(QLatin1String(File.readAll()));
    Window w;
    w.show();
    return a.exec();
}
