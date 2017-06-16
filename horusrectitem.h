#ifndef HORUSRECTITEM_H
#define HORUSRECTITEM_H
#include <QObject>
#include <QPointF>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsRectItem>

class HorusRectItem : public QObject, public QGraphicsRectItem{

    Q_OBJECT
public:
    HorusRectItem(qreal x, qreal y, qreal w, qreal h);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

signals:
    void lMouseDown(QPointF position);
    void lMouseUp();
    void mouseMoved(QPointF position);

private:
    bool dragging;
};

#endif // HORUSRECTITEM_H
