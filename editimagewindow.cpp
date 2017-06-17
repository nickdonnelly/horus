#include "editimagewindow.h"
#include "screenwindow.h"
#include "ui_editimagewindow.h"
#include <horusuploader.h>
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
#include  <QDialogButtonBox>
#include <QSlider>
#include <QLabel>


EditImageWindow::EditImageWindow(QString filename, HorusUploader * upl, QWidget *parent) :
    QMainWindow(parent),
    uploader(upl),
    ui(new Ui::EditImageWindow),
    fileLoc(filename)
{
    ui->setupUi(this);
    dragging = false;
    imagePixmap = new QPixmap(fileLoc);
    imgOriginalWidth = imagePixmap->width();
    imgOriginalHeight = imagePixmap->height();

    ui->btnBoxConfirm->setVisible(false);

    ui->sliderW->setMaximum(imgOriginalWidth);
    ui->sliderW->setMinimum(0);
    ui->sliderW->setValue(360);
    ui->sliderH->setMaximum(imgOriginalHeight);
    ui->sliderH->setMinimum(0);
    ui->sliderH->setValue(240);

    ui->lblWidth->setText("Crop Width: 360");
    ui->lblHeight->setText("Crop Height: 240");

    connect(ui->sliderH, SIGNAL(valueChanged(int)), this, SLOT(rectHeightChanged(int)));
    connect(ui->sliderW, SIGNAL(valueChanged(int)), this, SLOT(rectWidthChanged(int)));
    connect(ui->btnBox, SIGNAL(accepted()), this, SLOT(okPressed()));
    connect(ui->btnBox, SIGNAL(rejected()), this, SLOT(cancelPressed()));
    connect(ui->btnBoxConfirm, SIGNAL(accepted()), this, SLOT(confirmConfirmed()));
    connect(ui->btnBoxConfirm, SIGNAL(rejected()), this, SLOT(confirmCancelled()));


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
    showingCropped = false;
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
    // Holy inefficiency, batman!ore...
    float dx = position.x() - startX;
    float dy = position.y() - startY;
    QPointF rPos = rectangleItem->pos();
    float newX = std::max(0.0f, (float)rPos.x() + dx);
    float newY = std::max(0.0f, (float)rPos.y() + dy);
    if(rPos.x() + dx + rectangleItem->rect().width() > imgOriginalWidth){
        newX = (float)imgOriginalWidth - rectangleItem->rect().width();
    }

    if(rPos.y() + dy + rectangleItem->rect().height() > imgOriginalHeight){
        newY = (float)imgOriginalHeight - rectangleItem->rect().height();
    }

    rectangleItem->setPos(newX, newY);
    outlineItem->setPos(newX, newY);
}

void EditImageWindow::resizeEvent(QResizeEvent *evt){
    QMainWindow::resizeEvent(evt);
    if(showingCropped){
        ui->graphicsView->fitInView(croppedItem->boundingRect(), Qt::KeepAspectRatio);
        ui->graphicsView->centerOn(croppedItem);
    }else{
        ui->graphicsView->fitInView(imageItem->boundingRect(), Qt::KeepAspectRatio);
        ui->graphicsView->centerOn(imageItem);
    }
}

void EditImageWindow::paintEvent(QPaintEvent *evt){
    QMainWindow::paintEvent(evt);
}

void EditImageWindow::rectHeightChanged(int value){
    int rX, rY, rPosX, rPosY;
    rX = rectangleItem->rect().x();
    rY = rectangleItem->rect().y();

    rPosX = rectangleItem->pos().x();
    rPosY = rectangleItem->pos().y();

    ui->lblHeight->setText(QString("Crop Height: ") + QString::number(value));

    rectangleItem->setRect(rX, rY, rectangleItem->rect().width(), value);
    outlineItem->setRect(rX, rY, rectangleItem->rect().width(), value);
    if(value + rPosY > imgOriginalHeight){
        rectangleItem->setPos(rPosX, imgOriginalHeight - rectangleItem->rect().height());
        outlineItem->setPos(rPosX, imgOriginalHeight - rectangleItem->rect().height());
    }
}

void EditImageWindow::rectWidthChanged(int value){
    int rX, rY, rPosX, rPosY;
    rX = rectangleItem->rect().x();
    rY = rectangleItem->rect().y();

    rPosX = rectangleItem->pos().x();
    rPosY = rectangleItem->pos().y();

    ui->lblWidth->setText(QString("Crop Width: ") + QString::number(value));

    rectangleItem->setRect(rX, rY, value, rectangleItem->rect().height());
    outlineItem->setRect(rX, rY, value, rectangleItem->rect().height());
    if(value + rPosX > imgOriginalWidth){
        rectangleItem->setPos(imgOriginalWidth - rectangleItem->rect().width(), rPosY);
        outlineItem->setPos(imgOriginalWidth - rectangleItem->rect().width(), rPosY);
    }
}

void EditImageWindow::rectMouseDown(QPointF position){
    startX = position.x();
    startY = position.y();
    dragging = true;
}

void EditImageWindow::rectMouseUp(){
    dragging = false;
}

void EditImageWindow::okPressed(){
    int cropXPos, cropYPos, cropW, cropH;
    cropXPos = rectangleItem->pos().x();
    cropYPos = rectangleItem->pos().y();

    cropW = rectangleItem->rect().width();
    cropH = rectangleItem->rect().height();

    cropped = imagePixmap->copy(cropXPos, cropYPos, cropW, cropH);
    scene->removeItem(imageItem);
    scene->removeItem(rectangleItem);
    scene->removeItem(outlineItem);
    croppedItem = scene->addPixmap(cropped);
    showingCropped = true;
    ui->graphicsView->fitInView(croppedItem->boundingRect(), Qt::KeepAspectRatio);
    ui->graphicsView->centerOn(croppedItem);

    ui->btnBox->setVisible(false);
    ui->btnBoxConfirm->setVisible(true);
}

void EditImageWindow::cancelPressed(){
    this->close();
}

void EditImageWindow::confirmCancelled(){
    ui->btnBox->setVisible(true);
    ui->btnBoxConfirm->setVisible(false);

    scene->removeItem(croppedItem);
    scene->addItem(imageItem);
    scene->addItem(rectangleItem);
    scene->addItem(outlineItem);
    ui->graphicsView->fitInView(imageItem->boundingRect(), Qt::KeepAspectRatio);
    ui->graphicsView->centerOn(imageItem);
}

void EditImageWindow::confirmConfirmed(){
    hide();
    croppedFilename = ScreenWindow::getImagesDirectory() + "/" + ScreenWindow::getFilename(".png");
    cropped.save(croppedFilename);
    uploader->upload(false, croppedFilename);
}
