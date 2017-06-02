#include "screenwindow.h"
#include "ui_screenwindow.h"
#include "qdesktopwidget.h"
#include "qtextstream.h"
#include "QMouseEvent"
#include "QKeyEvent"

ScreenWindow::ScreenWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ScreenWindow)
{
    ui->setupUi(this);
//    QRect r = QApplication::desktop()->screenGeometry(1);
    QDesktopWidget * dtw = QApplication::desktop();
    windowW = dtw->screen()->width();
    windowH = dtw->screen()->height();
    ui->lblInstructions->resize(windowW, ui->lblInstructions->height());
    full = QRegion(0, 0, windowW, windowH);
    setParent(0); // no parent widget
    move(0, 0); // for windows...
    resize(windowW, windowH); // for windows...
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowState(Qt::WindowFullScreen);
    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
    setMask(full);


    showFullScreen();
    raise();
}

void ScreenWindow::keyPressEvent(QKeyEvent *evt){
    if(evt->key() == Qt::Key_Escape){
        close();
    }
}

 void ScreenWindow::mousePressEvent(QMouseEvent *evt){
    if(evt->button() == Qt::LeftButton){
        mousePressed = true;
        startX = evt->globalX();
        startY = evt->globalY();

        startXRel = evt->x();
        startYRel = evt->y();
    }else if(evt->button() == Qt::RightButton){
        close();
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
        update(); // repaint instruction
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
