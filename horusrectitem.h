#ifndef HORUSRECTITEM_H
#define HORUSRECTITEM_H
#include <QObject>
#include <QPointF>
#include <QBrush>
#include <QPen>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QWidget>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsRectItem>

class HorusRectItem : public QObject, public QGraphicsRectItem{

    Q_OBJECT
public:
    HorusRectItem(qreal x, qreal y, qreal w, qreal h);

    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget);

    enum Corner {
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight,
        None
    };

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

signals:
    void lMouseDown(QPointF position);
    void lMouseUp();
    void mouseMoved(QPointF position);

    void resizeFrom(float start, float end, HorusRectItem::Corner c);

private:
    bool dragging, resizing;
    int corner_diameter;
    QBrush ellipseBrush;
    QPen ellipsePen;
    Corner corner;

    QPointF start;

    Corner isInResizeArea(QPointF area);
};

#endif // HORUSRECTITEM_H
