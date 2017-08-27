#include "horustextitem.h"
#include <QGraphicsSceneMouseEvent>

HorusTextItem::HorusTextItem() : QGraphicsTextItem(),
    dragging(false)
{
}

void HorusTextItem::mousePressEvent(QGraphicsSceneMouseEvent *evt){
    anchorX = evt->pos().x();
    anchorY = evt->pos().y();
    dragging = true;
}

void HorusTextItem::mouseReleaseEvent(QGraphicsSceneMouseEvent){
    dragging = false;
}

void HorusTextItem::mouseMoveEvent(QGraphicsSceneMouseEvent *evt){
    if(dragging){
        float dx = evt->pos().x() - anchorX;
        float dy = evt->pos().y() - anchorY;

        QPointF txtPos = this->pos();
        // Left and top bounds. We'll ignore the other two since we don't know the
        // window bounds and there is an "infinite" number of possible text items anyway.
        float newX = std::max(0.0f, (float)txtPos.x() + dx);
        float newY = std::max(0.0f, (float)txtPos.y() + dy);

        this->setPos(QPointF(newX, newY));
    }
}
