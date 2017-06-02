#include "screenwindow.h"
#include "ui_screenwindow.h"
#include "qdesktopwidget.h"
#include "qtextstream.h"
#include "QMouseEvent"

ScreenWindow::ScreenWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ScreenWindow)
{
    ui->setupUi(this);
//    QRect r = QApplication::desktop()->screenGeometry(1);
    QDesktopWidget * dtw = QApplication::desktop();
    windowW = dtw->screen()->width();
    windowH = dtw->screen()->height();
    full = QRegion(0, 0, windowW, windowH);
    setParent(0);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowState(Qt::WindowFullScreen);
    setWindowFlags(Qt::WindowStaysOnTopHint);
    setMask(full);
#ifdef Q_OS_LINUX
//    setWindowFlags(Qt::X11BypassWindowManagerHint);
#endif
    setStyleSheet("background: rgba(0, 0, 0, 40);");
    showFullScreen();
    raise();
}

 void ScreenWindow::mousePressEvent(QMouseEvent *evt){
    if(evt->button() == Qt::LeftButton){
        mousePressed = true;
        startX = evt->globalX();
        startY = evt->globalY();

        startXRel = evt->x();
        startYRel = evt->y();
    }
 }

 void ScreenWindow::mouseReleaseEvent(QMouseEvent *evt){
    if(evt->button() == Qt::LeftButton){
        mousePressed = false;
        endX = evt->globalX();
        endY = evt->globalY();
        endXRel = evt->x();
        endYRel = evt->y();

        setMask(full);
    }
 }

 void ScreenWindow::mouseMoveEvent(QMouseEvent *evt){
    if(mousePressed){
        endX = evt->globalX();
        endY = evt->globalY();
        endXRel = evt->x();
        endYRel = evt->y();

        int originX = std::min(startXRel, endXRel);
        int originY = std::min(startYRel, endYRel);
        int iw = std::abs(endXRel - startXRel);
        int ih = std::abs(endYRel - startYRel);

        setMask(full - QRegion(originX, originY, iw, ih));
    }

 }


ScreenWindow::~ScreenWindow()
{
    delete ui;
}
