#ifndef EDITIMAGEWINDOW_H
#define EDITIMAGEWINDOW_H

#include <horusrectitem.h>
#include <horusgraphicsscene.h>
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


    void drawingModeToggled();
    void cancelPressed();
    void okPressed();
    void confirmConfirmed();
    void confirmCancelled();

    void clearPressed();
    void brushWidthChanged(int value);

    void colorRed(){ scene->setBrushColor(QColor(255, 0, 0, 255)); }
    void colorOrange(){ scene->setBrushColor(QColor(255, 165, 0, 255)); }
    void colorBlack(){ scene->setBrushColor(QColor(0, 0, 0, 255)); }
    void colorBlue(){ scene->setBrushColor(QColor(0, 0, 255, 255)); }
    void colorGreen(){ scene->setBrushColor(QColor(0, 255, 0, 255)); }
    void colorPink(){ scene->setBrushColor(QColor(255, 0, 255, 255)); }


private:
    Ui::EditImageWindow *ui;
    bool dragging, inDrawingMode, showingCropped;
    int imgOriginalWidth, imgOriginalHeight;
    float imgScaleFactor;
    int rdx, rdy, mx, my, mxOld, myOld;
    float startX, startY;

    HorusUploader * uploader;

    QString fileLoc, croppedFilename;
    QPixmap * imagePixmap;
    QPixmap cropped;
    HorusGraphicsScene * scene;
    QGraphicsItem *imageItem, *croppedItem;
    QGraphicsRectItem *outlineItem;
    HorusRectItem *rectangleItem;


protected:
    void paintEvent(QPaintEvent *evt);
    void resizeEvent(QResizeEvent *evt);
};

#endif // EDITIMAGEWINDOW_H
