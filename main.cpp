#include "screenwindow.h"
#include "horus.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //ScreenWindow w;
    //w.show();
    a.setQuitOnLastWindowClosed(false);
    Horus h;

//    h.hide();

    return a.exec();
}
