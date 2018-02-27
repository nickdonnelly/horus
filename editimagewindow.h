#ifndef EDITIMAGEWINDOW_H
#define EDITIMAGEWINDOW_H

#include <horusrectitem.h>
#include <horusgraphicsscene.h>
#include <horuspixmapgraphicsitem.h>
#include <horusuploader.h>
#include <QMainWindow>
#include <QString>
#include <QGraphicsSceneWheelEvent>
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
    void rectMouseResize(float dx, float dy, HorusRectItem::Corner c);

    void panStart(QPointF position);
    void panEnd();
    void panMove(QPointF position);

    void openImage();

    void drawingModeToggled();
    void cancelPressed();
    void okPressed();
    void confirmConfirmed();
    void confirmCancelled();
    void addTextPressed();

    void clearPressed();
    void brushWidthChanged(int value);

    void colorRed(){ scene->setBrushColor(QColor(255, 0, 0, 255)); setSelectedColor(HColor::Red); }
    void colorOrange(){ scene->setBrushColor(QColor(255, 165, 0, 255)); setSelectedColor(HColor::Orange); }
    void colorBlack(){ scene->setBrushColor(QColor(0, 0, 0, 255)); setSelectedColor(HColor::Black); }
    void colorBlue(){ scene->setBrushColor(QColor(0, 0, 255, 255)); setSelectedColor(HColor::Blue); }
    void colorGreen(){ scene->setBrushColor(QColor(0, 255, 0, 255)); setSelectedColor(HColor::Green); }
    void colorPink(){ scene->setBrushColor(QColor(255, 0, 255, 255)); setSelectedColor(HColor::Pink); }

    void scrolled(QGraphicsSceneWheelEvent *evt);


private:
    Ui::EditImageWindow *ui;
    bool dragging, inDrawingMode, showingCropped;
    int imgOriginalWidth, imgOriginalHeight;
    float imgScaleFactor;
    int rdx, rdy, mx, my, mxOld, myOld;
    float startX, startY, startPanX, startPanY;
    float zoom_coef;
    int zoom_count;

    HorusUploader * uploader;

    QString fileLoc, croppedFilename;
    QPixmap * imagePixmap;
    QPixmap cropped;
    HorusGraphicsScene * scene;
    QGraphicsItem *croppedItem;
    HorusPixmapGraphicsItem *imageItem;
    QGraphicsRectItem *outlineItem;
    HorusRectItem *rectangleItem;

    enum HColor {
        Red, Orange, Black, Blue, Green, Pink
    };
    void setSelectedColor(HColor color);
    void enforceRectangleBound();



protected:
    void paintEvent(QPaintEvent *evt);
    void resizeEvent(QResizeEvent *evt);
};

#endif // EDITIMAGEWINDOW_H
