#include <window/screenwindow.h>
#include "horus.h"
#include <horusstyle.h>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationDomain("horus.donnelly.cc");
    a.setOrganizationName("Donnelly.cc");

    // styles
    QFile File(":/res/master.qss");
    File.open(QFile::ReadOnly);
    QString _sheet = QLatin1String(File.readAll());
    a.setStyle(new HorusStyle);
    a.setStyleSheet(_sheet);

    a.setApplicationDisplayName("Horus");
    a.setApplicationName("Horus");
    a.setQuitOnLastWindowClosed(false);
    Horus h;

//    h.hide();

    return a.exec();
}
