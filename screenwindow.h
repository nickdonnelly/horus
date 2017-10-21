#ifndef SCREENWINDOW_H
#define SCREENWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QScreen>
#include <QPixmap>
#include <QGraphicsScene>
#include <QProcess>
#include <horusuploader.h>

namespace Ui {
class ScreenWindow;
}

class ScreenWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ScreenWindow(QScreen * screen, HorusUploader * u, int vidDuration = -1, QWidget *parent = 0);
    ~ScreenWindow();
    QString getLastSaveLocation();
    static QString getAppSaveDirectory();
    static QString getFilename(const QString ext);
    static QString getImagesDirectory();

private:
    Ui::ScreenWindow *ui;
    bool useVideo;

    QGraphicsScene * modeScene;
    QGraphicsPixmapItem *curItem;
    QPixmap pixModeVideo, pixModeImage;

    HorusUploader * uploader;
    QRect windowScreen;
    QScreen * wScreen;
    QRegion full;
    int originX, originY, originXRel, originYRel;
    int videoDuration;
    int iw, ih;
    int windowW, windowH;
    int mouseX, mouseY;
    int startX, startXRel;
    int startY, startYRel;
    int endX, endXRel;
    int endY, endYRel;
    bool mousePressed = false;
    QString lastSaveLocation, fileStr;
    void takeScreenshot();
    void takeScreenshot(int x, int y, int w, int h);
    void takeVideo(int duration, int vX, int vY, int vWidth, int vHeight);

public slots:
    void ffmpegFinished(int exitCode, QProcess::ExitStatus status);

signals:
    void closing();
    void recordStarted();
    void recordEnded(int exitCode);
    void stillTaken(QPixmap still);

protected:
    void paintEvent(QPaintEvent *evt);
    void keyPressEvent(QKeyEvent *evt);
    void mousePressEvent(QMouseEvent *evt);
    void mouseReleaseEvent(QMouseEvent *evt);
    void mouseMoveEvent(QMouseEvent *evt);

    void closeEvent(QCloseEvent*);
};

#endif // SCREENWINDOW_H
