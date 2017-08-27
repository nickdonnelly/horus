#ifndef HORUSTEXTITEM_H
#define HORUSTEXTITEM_H

#include <QObject>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsTextItem>
#include <QGraphicsObject>

class HorusTextItem : public QGraphicsTextItem
{
public:
    HorusTextItem();

signals:

public slots:

private:
    bool dragging;
    int anchorX, anchorY;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *evt);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *evt);
};

#endif // HORUSTEXTITEM_H
