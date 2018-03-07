#include "horusrectitem.h"
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsRectItem>
#include <QPoint>
#include <QTextStream>
#include <QPainterPath>

HorusRectItem::HorusRectItem(qreal x, qreal y, qreal w, qreal h)
    : QGraphicsRectItem(x,y,w,h)
{
    setAcceptHoverEvents(true);
    dragging = false;
    resizing = false;
    corner_diameter = 80;
    ellipseBrush = QBrush(Qt::cyan);
    ellipseBrushHover = QBrush(Qt::magenta);
    hover = false;
}

void HorusRectItem::paint(QPainter *painter,
                          const QStyleOptionGraphicsItem *option,
                          QWidget *widget)
{
     // Render from parent
    QGraphicsRectItem::paint(painter, option, widget);

    painter->setOpacity(0.70);

    int cw = corner_diameter / 2;

    QPainterPath tl, tr, bl, br;

    QPointF bottomRight = this->rect().bottomRight();

    br.moveTo(bottomRight.x(), bottomRight.y());
    br.arcTo(bottomRight.x() - cw, bottomRight.y() - cw, corner_diameter, corner_diameter, -180, -90);
    br.closeSubpath();

    if(hover) {
        painter->fillPath(br, ellipseBrushHover);
    } else {
        painter->fillPath(br, ellipseBrush);
    }
}

void HorusRectItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    hover = isInResizeArea(event->pos()) == Corner::BottomRight;
    update(); // force a repaint.
}

void HorusRectItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event)
    hover = false;
    update();
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
