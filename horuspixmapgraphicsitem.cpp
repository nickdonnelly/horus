#include "horuspixmapgraphicsitem.h"

HorusPixmapGraphicsItem::HorusPixmapGraphicsItem(const QPixmap &pixmap) : QGraphicsPixmapItem(pixmap)
{
    dragging = false;
}

void HorusPixmapGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent *evt)
{
    if(dragging) {
        emit mouseMoved(evt->pos());
    }
}

void HorusPixmapGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *evt)
{
    if(evt->button() == Qt::MouseButton::LeftButton) {
        dragging = true;
        emit lMouseDown(evt->pos());
    }
}

void HorusPixmapGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *evt)
{
    if(evt->button() == Qt::MouseButton::LeftButton){
        dragging = false;
        emit lMouseUp();
    }
}
