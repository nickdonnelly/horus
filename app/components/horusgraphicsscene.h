#ifndef HORUSGRAPHICSSCENE_H
#define HORUSGRAPHICSSCENE_H

#include <components/horustextitem.h>
#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneWheelEvent>
#include <QGraphicsItem>
#include <QGraphicsLineItem>
#include <QPolygon>
#include <QPainterPath>
#include <QColor>

class HorusGraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit HorusGraphicsScene(QObject *parent = 0);
    void setDrawingMode(bool mode);
    void setBrushSize(int brushSize);
    void setBrushColor(QColor color);
    void clearDrawing();
    void setImgRef(QGraphicsItem *ref);
    void addNewText(QString content, QPointF loc);

signals:
    void enterTextEdit();
    void exitTextEdit();
    void scrollEvent(QGraphicsSceneWheelEvent*);


private slots:
    void textEdit();
    void stopTextEdit();


private:
    QPolygon poly;
    QPainterPath path;
    QColor brush_color;
    QGraphicsItem *pathItem, *imgRef;
    QGraphicsLineItem *indicatorPoint;

    QList<QGraphicsItem*> points, pathItems;
    QList<HorusTextItem*> text;

    bool dragging;
    bool isDrawingMode;
    int brush_width;

    void removeTextItems();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *evt);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *evt);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *evt);
    void wheelEvent(QGraphicsSceneWheelEvent *evt);
};

#endif // HORUSGRAPHICSSCENE_H
