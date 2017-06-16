#include "editimagewindow.h"
#include "ui_editimagewindow.h"
#include <horusrectitem.h>
#include <QString>
#include <QResizeEvent>
#include <QPainter>
#include <QPoint>
#include <QPaintEvent>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QTextStream>
#include <QMouseEvent>
#include <QGuiApplication>
#include <QThread>
#include <QBrush>
#include <QPen>
#include <QScreen>
#include <QRect>
#include <QSlider>


EditImageWindow::EditImageWindow(QString filename, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::EditImageWindow),
    fileLoc(filename)
{
    ui->setupUi(this);
    dragging = false;
    imagePixmap = new QPixmap(fileLoc);
    imgOriginalWidth = imagePixmap->width();
    imgOriginalHeight = imagePixmap->height();
    ui->sliderW->setMaximum(imgOriginalWidth);
    ui->sliderW->setMinimum(0);
    ui->sliderH->setMaximum(imgOriginalHeight);
    ui->sliderH->setMinimum(0);

    connect(ui->sliderH, SIGNAL(valueChanged(int)), this, SLOT(rectHeightChanged(int)));
    connect(ui->sliderW, SIGNAL(valueChanged(int)), this, SLOT(rectWidthChanged(int)));

    scene = new QGraphicsScene(this);
    scene->setSceneRect(ui->graphicsView->x(), ui->graphicsView->y(), ui->graphicsView->width(), ui->graphicsView->height());
    imageItem =  scene->addPixmap(*imagePixmap);
    rectangleItem = new HorusRectItem(0, 0, 360, 240);
    scene->addItem(rectangleItem);
    //rectangleItem = scene->addRect(0, 0, 360, 240);
    rectangleItem->setBrush(QBrush(QColor(0, 0, 255, 80)));
    outlineItem = scene->addRect(0, 0, 360, 240);
    outlineItem->setPen(QPen(Qt::blue));
    ui->graphicsView->setScene(scene);
    ui->graphicsView->scene()->setSceneRect(ui->graphicsView->rect());
    ui->graphicsView->fitInView(imageItem->boundingRect(), Qt::IgnoreAspectRatio);
    ui->graphicsView->centerOn(imageItem);
    show();
    QThread::msleep(50);
    this->resize(this->width() + 1, this->height() + 1); // trigger the resize event
    QRect r = QGuiApplication::primaryScreen()->geometry();
    this->move(r.width() / 2 - this->width() / 2, r.height() / 2 - this->height() / 2);


    connect(rectangleItem, SIGNAL(lMouseDown()), this, SLOT(rectMouseDown()));
    connect(rectangleItem, SIGNAL(lMouseUp()), this, SLOT(rectMouseUp()));
    connect(rectangleItem, SIGNAL(mouseMoved(QPointF)), this, SLOT(rectMoved(QPointF)));
}

EditImageWindow::~EditImageWindow()
{
    delete ui;
}

void EditImageWindow::rectMoved(QPointF position){
    // Holy inefficiency, batman!
    float dx = position.x() - startX;
    float dy = position.y() - startY;
    QPointF rPos = rectangleItem->pos();
    rectangleItem->setPos(rPos.x() + dx, rPos.y() + dy);

}

void EditImageWindow::resizeEvent(QResizeEvent *evt){
    QMainWindow::resizeEvent(evt);
    //QPoint pos;
    //pos.setX((scene->width()/2) - (imageItem->boundingRect().width()/2));
    //pos.setY((scene->height()/2) - (imageItem->boundingRect().height()/2));
    ui->graphicsView->fitInView(imageItem->boundingRect(), Qt::KeepAspectRatio);
    ui->graphicsView->centerOn(imageItem);

    //imgScaleFactor = (float)imgOriginalWidth / (float)imageItem->boundingRect().width();
}

void EditImageWindow::paintEvent(QPaintEvent *evt){
    QMainWindow::paintEvent(evt);
    //QPainter painter(ui->frameImage);
    //painter.drawPixmap(0, 0, 150, 200, *imagePixmap);
}

void EditImageWindow::rectHeightChanged(int value){
    int rX, rY;
    rX = rectangleItem->pos().x();
    rY = rectangleItem->pos().y();
    rectangleItem->setRect(rX, rY, rectangleItem->rect().width(), value);
    outlineItem->setRect(rX, rY, rectangleItem->rect().width(), value);
}

void EditImageWindow::rectWidthChanged(int value){
    int rX, rY;
    rX = rectangleItem->pos().x();
    rY = rectangleItem->pos().y();
    rectangleItem->setRect(rX, rY, value, rectangleItem->rect().height());
    outlineItem->setRect(rX, rY, value, rectangleItem->rect().height());
}

void EditImageWindow::rectMouseDown(QPointF position){
    startX = position.x();
    startY = position.y();
    dragging = true;
}

void EditImageWindow::rectMouseUp(){
    dragging = false;
}
