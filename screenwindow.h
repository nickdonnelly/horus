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
    QRegion full;
    int windowW, windowH;
    int mouseX, mouseY;
    int startX, startXRel;
    int startY, startYRel;
    int endX, endXRel;
    int endY, endYRel;
    bool mousePressed = false;

protected:
    void mousePressEvent(QMouseEvent *evt);
    void mouseReleaseEvent(QMouseEvent *evt);
    void mouseMoveEvent(QMouseEvent *evt);
};

#endif // SCREENWINDOW_H
