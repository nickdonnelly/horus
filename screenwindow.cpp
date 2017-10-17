#include <screenwindow.h>
#include <horus.h>
#include <horusuploader.h>
#include <imagehelper.h>
#include <ui_screenwindow.h>
#include "qtextstream.h"
#include <QInputDialog>
#include <QDesktopWidget>
#include <QProcess>
#include <QWindow>
#include <QMouseEvent>
#include <QDir>
#include <QKeyEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QScreen>
#include <QSettings>
#include <QDateTime>
#include <QStandardPaths>
#include <QThread>
#ifdef Q_OS_WIN
#include <windows.h>
#endif
ScreenWindow::ScreenWindow(QScreen* screen, HorusUploader *u, int vidDuration, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ScreenWindow),
    uploader(u),
    videoDuration(vidDuration)
{
    ui->setupUi(this);
//    QRect r = QApplication::desktop()->screenGeometry(1);
    useVideo = videoDuration != -1;
    //this->windowHandle()->setScreen()

    wScreen = screen;
    windowScreen = screen->geometry();
    windowW = windowScreen.width();
    windowH = windowScreen.height();

    ui->lblInstructions->resize(windowW, ui->lblInstructions->height());
    full = QRegion(0, 0, windowW, windowH);
    setParent(0); // no parent widget
    move(windowScreen.x(), windowScreen.y()); // for windows...
    resize(windowW, windowH); // for windows...
    setAttribute(Qt::WA_TranslucentBackground);
    //setWindowState(Qt::WindowFullScreen);

#ifdef Q_OS_WIN
    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
#elif defined Q_OS_LINUX
    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::X11BypassWindowManagerHint);
#endif
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
        originXRel = std::min(startXRel, endXRel);
        originYRel = std::min(startYRel, endYRel);
        originX = std::min(startX, endX);
        originY = std::min(startY, endY);
        iw = std::abs(endXRel - startXRel);
        ih = std::abs(endYRel - startYRel);

        if(useVideo){
            // Get the desired duration

            takeVideo(videoDuration, originX, originY, iw ,ih);
        }else{
            takeScreenshot(originX, originY, iw, ih);
            takeScreenshot();
        }
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

        originXRel = std::min(startXRel, endXRel);
        originYRel = std::min(startYRel, endYRel);
        originX = std::min(startX, endX);
        originY = std::min(startY, endY);
        iw = std::abs(endXRel - startXRel);
        ih = std::abs(endYRel - startYRel);

        setMask(full - QRegion(originXRel, originYRel, iw, ih));
        update();
    }

 }

 void ScreenWindow::paintEvent(QPaintEvent *evt){
     QMainWindow::paintEvent(evt);
     if(mousePressed){
         QPainter painter(this);
         painter.setPen(QPen(Qt::blue));
         painter.drawRect(originXRel-1, originYRel-1, iw+2, ih+2);
     }
 }

void ScreenWindow::takeScreenshot(){
    hide();
    QDesktopWidget* s = QApplication::desktop();
    QRect compositeGeometry;
    for(int i = 0; i < s->screenCount(); ++i){
        QRect screenGeometry = s->screen(i)->geometry();
        compositeGeometry = compositeGeometry.united(screenGeometry);
    }

    QPixmap screenMap = QPixmap::grabWindow(s->winId(), compositeGeometry.x(), compositeGeometry.y(),
                        compositeGeometry.width(), compositeGeometry.height());

    if(!QDir(getAppSaveDirectory()).exists()){
        QDir().mkpath(getAppSaveDirectory());
    }

    QFile file(getAppSaveDirectory() + "/cache_last_taken.png");
    file.open(QIODevice::WriteOnly);
    screenMap.save(&file, "PNG");
    file.close();
    close();
}

 void ScreenWindow::takeScreenshot(int x, int y, int w, int h){
     // Make the window invisible
     hide();

     // Calculate overall screen geometry (this is necessary for multi-monitor setups)
     QDesktopWidget* s = QApplication::desktop();
     QRect compositeGeometry; // This holds the geometry of all screens combined.
     for(int i = 0; i < s->screenCount(); ++i){
         QRect screenGeometry = s->screen(i)->geometry();
         compositeGeometry = compositeGeometry.united(screenGeometry);
     }
     // Take screenshot
     QPixmap screenMap = wScreen->grabWindow(0, x, y, w, h);
     emit stillTaken(screenMap);
     // Save image
     lastSaveLocation = getImagesDirectory() + "/" + getFilename(".png");
     QFile file(lastSaveLocation);
     file.open(QIODevice::WriteOnly);
     screenMap.save(&file, "PNG");
     // Upload
     uploader->upload(false, lastSaveLocation);
     // Remove window
     close();
 }

 void ScreenWindow::takeVideo(int duration, int vX, int vY, int vWidth, int vHeight){
     hide();
     QProcess * process = new QProcess();
     QString processStr("");
     fileStr = QString("");
     fileStr += " \"" + getImagesDirectory() + "/" + getFilename(".webm\"");

#ifdef Q_OS_WIN
     processStr += "bin/ffmpeg.exe -f gdigrab -video_size ";
     processStr += QString::number(vWidth);
     processStr += "x";
     processStr += QString::number(vHeight);
     processStr += " -offset_x ";
     processStr += QString::number(vX);
     processStr += " -offset_y ";
     processStr += QString::number(vY);
     processStr += " -framerate 15 -i desktop -t ";
     processStr += QString::number(duration);
     processStr += fileStr;

#else
     processStr += "ffmpeg -f x11grab -framerate 15 -video_size ";
     processStr += QString::number(vWidth);
     processStr += "x";
     processStr += QString::number(vHeight);
     processStr += " -i :0.0+" + QString::number(vX) + "," + QString::number(vY) + " -t " + QString::number(duration);
     processStr += " -vf \"format=rgba\"";
     processStr += fileStr;

#endif
     emit recordStarted();
     connect(process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(ffmpegFinished(int, QProcess::ExitStatus)));
     process->start(processStr);
 }

 // TODO: Fix this not handling the copy mode setting correctly when set to resource rather than URL/none.
 void ScreenWindow::ffmpegFinished(int exitCode, QProcess::ExitStatus){
     emit recordEnded(exitCode);
     QThread::msleep(500); // let the file handles close so exists() returns true.
     if(exitCode == 0){
        uploader->upload(true, fileStr.replace("\"", ""));
     }
     close();
 }


 QString ScreenWindow::getLastSaveLocation(){
     return lastSaveLocation;
 }


QString ScreenWindow::getImagesDirectory(){
    QSettings sets("horus-settings.ini", QSettings::IniFormat);
    QString picPath = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation) + "/horus";
    QString setDir = sets.value("other/saveDirectory", picPath).toString();

    if(!QDir(setDir).exists()){
        QDir().mkdir(setDir);
    }
    return setDir;
}

QString ScreenWindow::getAppSaveDirectory(){
   return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

}

QString ScreenWindow::getFilename(const QString ext){
    QDateTime dt = QDateTime::currentDateTime();
    return dt.toString("dd-MMM-yyyy HH-mm-ss") + ext;
}

ScreenWindow::~ScreenWindow()
{
    delete ui;
}

void ScreenWindow::closeEvent(QCloseEvent*){
    emit closing();
}
