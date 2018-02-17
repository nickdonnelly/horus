#include "horusrectitem.h"
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsRectItem>
#include <QPoint>
#include <QTextStream>
#include <QPainterPath>

HorusRectItem::HorusRectItem(qreal x, qreal y, qreal w, qreal h)
    : QGraphicsRectItem(x,y,w,h)
{
    dragging = false;
    resizing = false;
    corner_diameter = 80;
    ellipseBrush = QBrush(Qt::cyan);
    ellipsePen = QPen(Qt::darkCyan, 3.0f);
}

void HorusRectItem::paint(QPainter *painter,
                          const QStyleOptionGraphicsItem *option,
                          QWidget *widget)
{
     // Render from parent
    QGraphicsRectItem::paint(painter, option, widget);

    painter->setOpacity(0.70);
    painter->setBrush(ellipseBrush);
    painter->setPen(ellipsePen);
    int cw = corner_diameter / 2;

    QPainterPath tl, tr, bl, br;

    QPointF topLeft = this->rect().topLeft();
    QPointF topRight = this->rect().topRight();
    QPointF bottomLeft = this->rect().bottomLeft();
    QPointF bottomRight = this->rect().bottomRight();

    /* We only use bottom right for now.
    tl.moveTo(topLeft.x(), topLeft.y());
    tl.arcTo(topLeft.x() - cw, topLeft.y() - cw, corner_diameter, corner_diameter, 0, -90);
    tl.closeSubpath();

    tr.moveTo(topRight.x(), topRight.y());
    tr.arcTo(topRight.x() - cw, topRight.y() - cw, corner_diameter, corner_diameter, -180, 90);
    tr.closeSubpath();

    bl.moveTo(bottomLeft.x(), bottomLeft.y());
    bl.arcTo(bottomLeft.x() - cw, bottomLeft.y() - cw, corner_diameter, corner_diameter, 0, 90);
    bl.closeSubpath();
    */

    br.moveTo(bottomRight.x(), bottomRight.y());
    br.arcTo(bottomRight.x() - cw, bottomRight.y() - cw, corner_diameter, corner_diameter, -180, -90);
    br.closeSubpath();

    painter->fillPath(tl, ellipseBrush);
    painter->fillPath(tr, ellipseBrush);
    painter->fillPath(bl, ellipseBrush);
    painter->fillPath(br, ellipseBrush);

    // Top left corner
    //painter->drawEllipse(-1 * cw, -1 * cw, corner_diameter, corner_diameter);
    // Top right corner
    //painter->drawEllipse(w - cw, -1 * cw, corner_diameter, corner_diameter);
    // Bottom left corner
    //painter->drawEllipse(-1 * cw, h - cw, corner_diameter, corner_diameter);
    // Bottom right corner
    //painter->drawEllipse(w - cw, h - cw, corner_diameter, corner_diameter);
}

void HorusRectItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(dragging){
        emit mouseMoved(event->pos());
    } else if(resizing) {
        emit resizeFrom(start.x() - event->pos().x(), start.y() - event->pos().y(), corner);
        start = event->pos();
    }
}

void HorusRectItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::MouseButton::LeftButton){

        Corner c = isInResizeArea(event->pos());
        if(c == Corner::BottomRight) {
            resizing = true;
            corner = c;
            start = event->pos();
        } else {
            dragging = true;
            emit lMouseDown(event->pos());
        }
    }
}

void HorusRectItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::MouseButton::LeftButton){
        dragging = false;
        resizing = false;
        emit lMouseUp();
    }
}


HorusRectItem::Corner HorusRectItem::isInResizeArea(QPointF area)
{
    float ax = area.x();
    float ay = area.y();
    int w = this->rect().width();
    int h = this->rect().height();
    int x = this->rect().x();
    int y = this->rect().y();
    int cw = corner_diameter / 2;
    if (ax < x + cw && ay < y + cw){
        return Corner::TopLeft;
    } else if (ax > x + w - cw && ay < y + cw){
        return Corner::TopRight;
    } else if (ax < x + cw && ay > y + h - cw){
        return Corner::BottomLeft;
    } else if (ax > x + w - cw && ay > y + h - cw) {
        return Corner::BottomRight;
    } else {
        return Corner::None;
    }
}
