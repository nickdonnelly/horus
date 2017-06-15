#ifndef SCREENWINDOW_H
#define SCREENWINDOW_H

#include <QMainWindow>
#include <QString>
#include <horusuploader.h>

namespace Ui {
class ScreenWindow;
}

class ScreenWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ScreenWindow(QWidget *parent = 0);
    ScreenWindow(HorusUploader * u, int vidDuration = -1, QWidget *parent = 0);
    ~ScreenWindow();
    QString getLastSaveLocation();
    static QString getAppSaveDirectory();
    static QString getFilename(const QString ext);
    static QString getImagesDirectory();

private:
    Ui::ScreenWindow *ui;
    bool useVideo;
    HorusUploader * uploader;
    QWidget * windowScreen;
    QRegion full;
    int originX, originY;
    int videoDuration;
    int iw, ih;
    int windowW, windowH;
    int mouseX, mouseY;
    int startX, startXRel;
    int startY, startYRel;
    int endX, endXRel;
    int endY, endYRel;
    bool mousePressed = false;
    QString lastSaveLocation;
    void takeScreenshot();
    void takeScreenshot(int x, int y, int w, int h);
    void takeVideo(int duration, int vX, int vY, int vWidth, int vHeight);

signals:
    void recordStarted();
    void recordEnded();

protected:
    void paintEvent(QPaintEvent *evt);
    void keyPressEvent(QKeyEvent *evt);
    void mousePressEvent(QMouseEvent *evt);
    void mouseReleaseEvent(QMouseEvent *evt);
    void mouseMoveEvent(QMouseEvent *evt);
};

#endif // SCREENWINDOW_H
