#include "editimagewindow.h"
#include "ui_editimagewindow.h"
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

    scene = new QGraphicsScene(this);
    scene->setSceneRect(ui->graphicsView->x(), ui->graphicsView->y(), ui->graphicsView->width(), ui->graphicsView->height());
    imageItem =  scene->addPixmap(*imagePixmap);
    rectangleItem = scene->addRect(0, 0, 360, 240);
    rectangleItem->setBrush(QBrush(Qt::blue));
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
    //imageItem = scene->addPixmap(*imagePixmap);
    //imgItem->setPos(0, 0);
    //ui->graphicsView->setScene(scene);
}

EditImageWindow::~EditImageWindow()
{
    delete ui;
}

bool EditImageWindow::eventFilter(QObject *watched, QEvent *event){
    //QTextStream(stdout) << "EVENT FILTERED" << endl;
    if(watched == qobject_cast<QObject *>(ui->graphicsView)){
        if(event->type() == QEvent::MouseButtonPress){
            dragging = true;
        }else if(dragging && event->type() == QEvent::MouseMove){
            QMouseEvent * mev = static_cast<QMouseEvent *>(event);
            mx = mev->x();
            my = mev->y();
            QTextStream(stdout) << mx << " " << my << endl;
        }else if(dragging && event->type() == QEvent::MouseButtonRelease){
            dragging = false;
        }
    }
    return false;
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
