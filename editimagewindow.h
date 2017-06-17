#ifndef EDITIMAGEWINDOW_H
#define EDITIMAGEWINDOW_H

#include <horusrectitem.h>
#include <horusuploader.h>
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
    explicit EditImageWindow(QString filename, HorusUploader * upl, QWidget *parent = 0);
    ~EditImageWindow();

private slots:
    void rectWidthChanged(int value);
    void rectHeightChanged(int value);
    void rectMouseDown(QPointF position);
    void rectMouseUp();
    void rectMoved(QPointF position);

    void cancelPressed();
    void okPressed();
    void confirmConfirmed();
    void confirmCancelled();


private:
    Ui::EditImageWindow *ui;
    bool dragging, showingCropped;
    int imgOriginalWidth, imgOriginalHeight;
    float imgScaleFactor;
    int rdx, rdy, mx, my, mxOld, myOld;
    float startX, startY;

    HorusUploader * uploader;

    QString fileLoc, croppedFilename;
    QPixmap * imagePixmap;
    QPixmap cropped;
    QGraphicsScene * scene;
    QGraphicsItem *imageItem, *croppedItem;
    QGraphicsRectItem *outlineItem;
    HorusRectItem *rectangleItem;


protected:
    void paintEvent(QPaintEvent *evt);
    void resizeEvent(QResizeEvent *evt);
};

#endif // EDITIMAGEWINDOW_H
