#include "screenwindow.h"
#include "horus.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationDomain("horus.donnelly.cc");
    a.setOrganizationName("Donnelly.cc");
    a.setApplicationDisplayName("Horus");
    a.setApplicationName("Horus");
    //ScreenWindow w;
    //w.show();
    a.setQuitOnLastWindowClosed(false);
    Horus h;

//    h.hide();

    return a.exec();
}
