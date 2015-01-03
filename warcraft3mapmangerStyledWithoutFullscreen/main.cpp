#include <QApplication>
#include "CustomWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc,argv);
    QFile File(":stylesheet.qss");
    File.open(QFile::ReadOnly);
    app.setStyleSheet(QLatin1String(File.readAll()));
    CustomWindow *myWindow = new CustomWindow();
    myWindow->show();

    return app.exec();
}
