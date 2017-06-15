#ifndef EDITIMAGEWINDOW_H
#define EDITIMAGEWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QResizeEvent>
#include <QPaintEvent>
#include <QPixmap>
#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QGraphicsScene>

namespace Ui {
class EditImageWindow;
}

class EditImageWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit EditImageWindow(QString filename, QWidget *parent = 0);
    ~EditImageWindow();



private:
    Ui::EditImageWindow *ui;
    bool dragging;
    int imgOriginalWidth, imgOriginalHeight;
    float imgScaleFactor;
    int rdx, rdy, mx, my;

    QString fileLoc;
    QPixmap * imagePixmap;
    QGraphicsScene * scene;
    QGraphicsItem *imageItem;
    QGraphicsRectItem *rectangleItem, *outlineItem;

protected:
    void paintEvent(QPaintEvent *evt);
    void resizeEvent(QResizeEvent *evt);
    bool eventFilter(QObject *watched, QEvent *event);
};

#endif // EDITIMAGEWINDOW_H
