#include "screenwindow.h"
#include <horusuploader.h>
#include "ui_screenwindow.h"
#include "qdesktopwidget.h"
#include "qtextstream.h"
#include <QMouseEvent>
#include <QDir>
#include <QKeyEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QScreen>
#include <QDateTime>
#include <QStandardPaths>

ScreenWindow::ScreenWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ScreenWindow)
{
    ui->setupUi(this);
//    QRect r = QApplication::desktop()->screenGeometry(1);
    QDesktopWidget * dtw = QApplication::desktop();
    windowScreen = dtw->screen();
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
#ifdef Q_OS_LINUX
    setWindowFlags(Qt::X11BypassWindowManagerHint);
#endif
    setMask(full);

    showFullScreen();
    raise();
}


ScreenWindow::ScreenWindow(HorusUploader *u, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ScreenWindow),
    uploader(u)
{
    ui->setupUi(this);
//    QRect r = QApplication::desktop()->screenGeometry(1);
    QDesktopWidget * dtw = QApplication::desktop();
    windowScreen = dtw->screen();
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
    } }

 void ScreenWindow::mouseReleaseEvent(QMouseEvent *evt){
    if(evt->button() == Qt::LeftButton){
        mousePressed = false;
        endX = evt->globalX();
        endY = evt->globalY();
        endXRel = evt->x();
        endYRel = evt->y();

        setMask(full);
        update(); // repaint instruction
        originX = std::min(startXRel, endXRel);
        originY = std::min(startYRel, endYRel);
        iw = std::abs(endXRel - startXRel);
        ih = std::abs(endYRel - startYRel);

        takeScreenshot(originX, originY, iw, ih);
        takeScreenshot();
    }else if(evt->button() == Qt::RightButton){
        close();
    }

 }

 void ScreenWindow::mouseMoveEvent(QMouseEvent *evt){
    if(mousePressed){
        endX = evt->globalX();
        endY = evt->globalY();
        endXRel = evt->x();
        endYRel = evt->y();

        originX = std::min(startXRel, endXRel);
        originY = std::min(startYRel, endYRel);
        iw = std::abs(endXRel - startXRel);
        ih = std::abs(endYRel - startYRel);

        setMask(full - QRegion(originX, originY, iw, ih));
        update();
    }

 }

 void ScreenWindow::paintEvent(QPaintEvent *evt){
     QMainWindow::paintEvent(evt);
     if(mousePressed){
         QPainter painter(this);
         painter.setPen(QPen(Qt::blue));
         painter.drawRect(originX-1, originY-1, iw+2, ih+2);
     }
 }

 void ScreenWindow::takeScreenshot(){
      hide();
     // TODO: Use screens() and iterate through for a list in the future to splice the pixmaps together
     QPixmap screenMap = QGuiApplication::primaryScreen()->grabWindow(0, 0, 0, windowW, windowH);
     if(!QDir(getAppSaveDirectory()).exists()){
         QDir().mkpath(getAppSaveDirectory());
     }
     QFile file(getAppSaveDirectory() + "/cache_last_taken.png");
     file.open(QIODevice::WriteOnly);
     screenMap.save(&file, "PNG");
     close();
 }

 void ScreenWindow::takeScreenshot(int x, int y, int w, int h){
     hide();
     // TODO: Use screens() and iterate through for a list in the future to splice the pixmaps together
     QPixmap screenMap = QGuiApplication::primaryScreen()->grabWindow(0, x, y, w, h);
     lastSaveLocation = getImagesDirectory() + "/" + getFilename();
     QFile file(lastSaveLocation);
     file.open(QIODevice::WriteOnly);
     screenMap.save(&file, "PNG");
     uploader->upload(lastSaveLocation);
     close();
 }

 QString ScreenWindow::getLastSaveLocation(){
     return lastSaveLocation;
 }


QString ScreenWindow::getImagesDirectory(){
    if(!QDir(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation) + "/horus").exists()){
        QDir().mkdir(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation) + "/horus");
    }
    return QStandardPaths::writableLocation(QStandardPaths::PicturesLocation) + "/horus";
}

QString ScreenWindow::getAppSaveDirectory(){
   return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

}

QString ScreenWindow::getFilename(){
    QDateTime dt = QDateTime::currentDateTime();
    return dt.toString("dd-MMM-yyyy HH-mm-ss") + ".png";
}

ScreenWindow::~ScreenWindow()
{
    delete ui;
}
