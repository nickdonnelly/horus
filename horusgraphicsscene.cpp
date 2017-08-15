#include "horusgraphicsscene.h"
#include <QGraphicsScene>
#include <QGraphicsPathItem>
#include <QGraphicsLineItem>
#include <QPainterPath>
#include <QTextStream>
#include <QPolygon>
#include <QGraphicsSceneMouseEvent>
#include <QPoint>
#include <QPen>
#include <QList>
#include <QColor>

HorusGraphicsScene::HorusGraphicsScene(QObject *parent) : QGraphicsScene(parent),
    poly(), path(), points(), pathItems(), dragging(false), isDrawingMode(false)
{
    brush_color = Qt::red;
    brush_width = 15;
    indicatorPoint = addLine(0, 0, 0, 0, QPen(brush_color, brush_width));
    indicatorPoint->setVisible(false);
    indicatorPoint->setZValue(1);
}

void HorusGraphicsScene::setBrushColor(QColor color){
    brush_color = color;
    indicatorPoint->setPen(QPen(brush_color, brush_width));
}

void HorusGraphicsScene::setDrawingMode(bool mode){
    this->isDrawingMode = mode;
    if(isDrawingMode){
        indicatorPoint->setVisible(true);
    }else{
        indicatorPoint->setVisible(false);
    }
}

void HorusGraphicsScene::setBrushSize(int brushSize){
    brush_width = brushSize;
    indicatorPoint->setPen(QPen(brush_color, brush_width));
}

void HorusGraphicsScene::clearDrawing(){
    poly.clear();
    path = QPainterPath();
    for(int i = 0; i < pathItems.size(); i++){
        removeItem(pathItems.at(i));
        delete pathItems.at(i);
    }
    pathItems.clear();
}

void HorusGraphicsScene::setImgRef(QGraphicsItem *ref){
    imgRef = ref;
}

void HorusGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *evt){
    QGraphicsScene::mouseMoveEvent(evt);
    QPoint pos = evt->scenePos().toPoint();
    if(dragging){
        poly.append(pos);
        points << addLine(pos.x(), pos.y(), pos.x(), pos.y(), QPen(brush_color, brush_width));
    }
    indicatorPoint->setX(pos.x());
    indicatorPoint->setY(pos.y());
}

void HorusGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *evt){
    QGraphicsScene::mouseReleaseEvent(evt);
    if(isDrawingMode && evt->button() == Qt::LeftButton){
        dragging = false;
        path.addPolygon(poly);
        pathItems << addPath(path, QPen(brush_color, brush_width));
        path = QPainterPath();
        poly.clear();
        for(int i = 0; i < points.size(); i++){
            removeItem(points.at(i)); // clears the artifacts of the points and keeps performance.
            delete points.at(i);
        }
        points.clear();
    }
}

void HorusGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *evt){
    QGraphicsScene::mousePressEvent(evt);
    if(isDrawingMode && evt->button() == Qt::LeftButton){
        dragging = true;
    }
}

