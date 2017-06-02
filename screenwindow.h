#ifndef SCREENWINDOW_H
#define SCREENWINDOW_H

#include <QMainWindow>

namespace Ui {
class ScreenWindow;
}

class ScreenWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ScreenWindow(QWidget *parent = 0);
    ~ScreenWindow();

private:
    Ui::ScreenWindow *ui;
    QWidget * windowScreen;
    QRegion full;
    int originX, originY;
    int iw, ih;
    int windowW, windowH;
    int mouseX, mouseY;
    int startX, startXRel;
    int startY, startYRel;
    int endX, endXRel;
    int endY, endYRel;
    bool mousePressed = false;
    void takeScreenshot();
    void takeScreenshot(int x, int y, int w, int h);

protected:
    void paintEvent(QPaintEvent *evt);
    void keyPressEvent(QKeyEvent *evt);
    void mousePressEvent(QMouseEvent *evt);
    void mouseReleaseEvent(QMouseEvent *evt);
    void mouseMoveEvent(QMouseEvent *evt);
};

#endif // SCREENWINDOW_H
