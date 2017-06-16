#ifndef EDITIMAGEWINDOW_H
#define EDITIMAGEWINDOW_H

#include <horusrectitem.h>
#include <QMainWindow>
#include <QString>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPixmap>
#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QPointF>

namespace Ui {
class EditImageWindow;
}

class EditImageWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit EditImageWindow(QString filename, QWidget *parent = 0);
    ~EditImageWindow();

private slots:
    void rectWidthChanged(int value);
    void rectHeightChanged(int value);
    void rectMouseDown(QPointF position);
    void rectMouseUp();
    void rectMoved(QPointF position);


private:
    Ui::EditImageWindow *ui;
    bool dragging;
    int imgOriginalWidth, imgOriginalHeight;
    float imgScaleFactor;
    int rdx, rdy, mx, my, mxOld, myOld;
    float startX, startY;

    QString fileLoc;
    QPixmap * imagePixmap;
    QGraphicsScene * scene;
    QGraphicsItem *imageItem;
    QGraphicsRectItem *outlineItem;
    HorusRectItem *rectangleItem;


protected:
    void paintEvent(QPaintEvent *evt);
    void resizeEvent(QResizeEvent *evt);
};

#endif // EDITIMAGEWINDOW_H
