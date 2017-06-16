#include "horusrectitem.h"
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsRectItem>

HorusRectItem::HorusRectItem(qreal x, qreal y, qreal w, qreal h) : QGraphicsRectItem(x,y,w,h) {
    dragging = false;
}

void HorusRectItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event){
    if(dragging){
        emit mouseMoved(event->pos());
    }
}

void HorusRectItem::mousePressEvent(QGraphicsSceneMouseEvent *event){
    if(event->button() == Qt::MouseButton::LeftButton){
        dragging = true;
        emit lMouseDown(event->pos());
    }
}

void HorusRectItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){
    if(event->button() == Qt::MouseButton::LeftButton){
        dragging = false;
        emit lMouseUp();
    }
}

