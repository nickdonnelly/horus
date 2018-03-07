#ifndef HORUSPIXMAPGRAPHICSITEM_H
#define HORUSPIXMAPGRAPHICSITEM_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>
#include <QPixmap>

class HorusPixmapGraphicsItem : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    HorusPixmapGraphicsItem(const QPixmap &pixmap);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *evt);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *evt);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *evt);

signals:
    void lMouseDown(QPointF position);
    void lMouseUp();
    void mouseMoved(QPointF position);

private:
    bool dragging;
};

#endif // HORUSPIXMAPGRAPHICSITEM_H
