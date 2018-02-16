#include "editimagewindow.h"
#include "screenwindow.h"
#include "ui_editimagewindow.h"
#include <horusuploader.h>
#include "horusgraphicsscene.h"
#include <horusrectitem.h>
#include <QString>
#include <QResizeEvent>
#include <QPainter>
#include <QPoint>
#include <QPaintEvent>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QTextStream>
#include <QMouseEvent>
#include <QGuiApplication>
#include <QThread>
#include <QBrush>
#include <QPen>
#include <QScreen>
#include <QRect>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QSlider>
#include <QLabel>


EditImageWindow::EditImageWindow(QString filename, HorusUploader * upl, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::EditImageWindow),
    uploader(upl),
    fileLoc(filename),
    zoom_coef(1.35),
    zoom_count(0)
{
    ui->setupUi(this);
    ui->graphicsView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    dragging = false;
    imagePixmap = new QPixmap(fileLoc);
    imgOriginalWidth = imagePixmap->width();
    imgOriginalHeight = imagePixmap->height();

    ui->btnBoxConfirm->setVisible(false);
    ui->sliderBrushSize->setVisible(false);
    ui->lblBrushSize->setVisible(false);
    ui->btnClear->setVisible(false);

    ui->sliderW->setMaximum(imgOriginalWidth);
    ui->sliderW->setMinimum(1);
    ui->sliderW->setValue(360);
    ui->sliderH->setMaximum(imgOriginalHeight);
    ui->sliderH->setMinimum(1);
    ui->sliderH->setValue(240);

    ui->lblWidth->setText("Crop Width: 360");
    ui->lblHeight->setText("Crop Height: 240");



    connect(ui->sliderH, SIGNAL(valueChanged(int)), this, SLOT(rectHeightChanged(int)));
    connect(ui->sliderW, SIGNAL(valueChanged(int)), this, SLOT(rectWidthChanged(int)));
    connect(ui->sliderBrushSize, SIGNAL(valueChanged(int)), this, SLOT(brushWidthChanged(int)));
    connect(ui->btnBox, SIGNAL(accepted()), this, SLOT(okPressed()));
    connect(ui->btnBox, SIGNAL(rejected()), this, SLOT(cancelPressed()));
    connect(ui->btnBoxConfirm, SIGNAL(accepted()), this, SLOT(confirmConfirmed()));
    connect(ui->btnBoxConfirm, SIGNAL(rejected()), this, SLOT(confirmCancelled()));
    connect(ui->btnDrawMode, SIGNAL(pressed()), this, SLOT(drawingModeToggled()));
    connect(ui->btnClear, SIGNAL(pressed()), this, SLOT(clearPressed()));
    connect(ui->btnAddText, SIGNAL(pressed()), this, SLOT(addTextPressed()));

    // color buttons
    connect(ui->btnBlack, SIGNAL(pressed()), this, SLOT(colorBlack()));
    connect(ui->btnOrange, SIGNAL(pressed()), this, SLOT(colorOrange()));
    connect(ui->btnBlue, SIGNAL(pressed()), this, SLOT(colorBlue()));
    connect(ui->btnPink, SIGNAL(pressed()), this, SLOT(colorPink()));
    connect(ui->btnGreen, SIGNAL(pressed()), this, SLOT(colorGreen()));
    connect(ui->btnRed, SIGNAL(pressed()), this, SLOT(colorRed()));

    // Scene preparation
    scene = new HorusGraphicsScene(this);
    connect(scene, SIGNAL(scrollEvent(QGraphicsSceneWheelEvent*)), this, SLOT(scrolled(QGraphicsSceneWheelEvent*)));
    scene->setSceneRect(ui->graphicsView->x(), ui->graphicsView->y(), ui->graphicsView->width(), ui->graphicsView->height());


    imageItem = new HorusPixmapGraphicsItem(*imagePixmap);
    scene->addItem(imageItem);

    rectangleItem = new HorusRectItem(0, 0, 360, 240);
    scene->setImgRef(rectangleItem);
    scene->addItem(rectangleItem);
    rectangleItem->setBrush(QBrush(QColor(0, 0, 255, 80)));
    rectangleItem->setZValue(100);
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


    // Rectangle Mouse Events
    connect(rectangleItem, SIGNAL(lMouseDown(QPointF)), this, SLOT(rectMouseDown(QPointF)));
    connect(rectangleItem, SIGNAL(lMouseUp()), this, SLOT(rectMouseUp()));
    connect(rectangleItem, SIGNAL(mouseMoved(QPointF)), this, SLOT(rectMoved(QPointF)));

    // Image mouse events
    connect(imageItem, SIGNAL(lMouseDown(QPointF)), this, SLOT(panStart(QPointF)));
    connect(imageItem, SIGNAL(lMouseUp()), this, SLOT(panEnd()));
    connect(imageItem, SIGNAL(mouseMoved(QPointF)), this, SLOT(panMove(QPointF)));

}

EditImageWindow::~EditImageWindow()
{
    delete ui;
}

void EditImageWindow::rectMoved(QPointF position){
    float dx = position.x() - startX;
    float dy = position.y() - startY;
    QPointF rPos = rectangleItem->pos();
    float newX = std::max(0.0f, (float)rPos.x() + dx);
    float newY = std::max(0.0f, (float)rPos.y() + dy);
    float scale = imageItem->scale();
    if(rPos.x() + dx + rectangleItem->rect().width() > (imgOriginalWidth * imageItem->scale())){
        newX = (float)imgOriginalWidth - rectangleItem->rect().width();
    }

    if(rPos.y() + dy + rectangleItem->rect().height() > (imgOriginalHeight * imageItem->scale())){
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

void EditImageWindow::panStart(QPointF position)
{
    startPanX = position.x();
    startPanY = position.y();
    dragging = true;
}

void EditImageWindow::panEnd()
{
    dragging = false;
}

void EditImageWindow::panMove(QPointF position)
{

    float dx = position.x() - startPanX;
    float dy = position.y() - startPanY;
    QPointF currentPos = imageItem->pos();
    float newX = (float) currentPos.x() + dx;
    float newY = (float) currentPos.y() + dy;

    imageItem->setPos(newX, newY);
}

void EditImageWindow::okPressed()
{
    int cropXPos, cropYPos, cropW, cropH;
    cropXPos = rectangleItem->pos().x();
    cropYPos = rectangleItem->pos().y();
    cropW = rectangleItem->rect().width();
    cropH = rectangleItem->rect().height();

    rectangleItem->setVisible(false);
    outlineItem->setVisible(false);
    cropped = QPixmap(cropW, cropH);
    QPainter * croppedPainter = new QPainter(&cropped);
    scene->render(croppedPainter, cropped.rect(), QRect(cropXPos, cropYPos, cropW, cropH), Qt::KeepAspectRatio);
    scene->clearDrawing();

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
    showingCropped = false;
}

void EditImageWindow::confirmConfirmed(){
    hide();
    croppedFilename = ScreenWindow::getImagesDirectory() + "/" + ScreenWindow::getFilename(".png");
    cropped.save(croppedFilename);
    uploader->upload(false, croppedFilename);
}

void EditImageWindow::drawingModeToggled(){
    if(inDrawingMode){
        inDrawingMode = false;

        ui->sliderBrushSize->setVisible(false);
        ui->lblBrushSize->setVisible(false);
        ui->sliderH->setVisible(true);
        ui->sliderW->setVisible(true);
        ui->lblWidth->setVisible(true);
        ui->lblHeight->setVisible(true);
        ui->btnClear->setVisible(false);
        ui->btnAddText->setVisible(true);
        ui->btnBox->setEnabled(true);

        rectangleItem->setVisible(true);
        outlineItem->setVisible(true);

        ui->btnDrawMode->setText("Enter Drawing Mode");
    }else{
        inDrawingMode = true;

        ui->sliderBrushSize->setVisible(true);
        ui->btnClear->setVisible(true);
        ui->btnBox->setEnabled(false);
        ui->lblBrushSize->setVisible(true);
        ui->sliderH->setVisible(false);
        ui->sliderW->setVisible(false);
        ui->lblWidth->setVisible(false);
        ui->lblHeight->setVisible(false);
        ui->btnAddText->setVisible(false);

        rectangleItem->setVisible(false);
        ui->btnDrawMode->setText("Exit Drawing Mode");
    }
    scene->setDrawingMode(inDrawingMode);
}

void EditImageWindow::brushWidthChanged(int value){
    scene->setBrushSize(value);
    ui->lblBrushSize->setText("Brush Size: " + QString::number(value));
}

void EditImageWindow::clearPressed(){
    scene->clearDrawing();
}

// TODO: Make this font/color editable so that it isnt always ugly and red.
void EditImageWindow::addTextPressed()
{
    scene->addNewText("Click to edit.", QPointF(0, 0));
}

void EditImageWindow::setSelectedColor(HColor color)
{
    QPushButton *buttons[6] = {
        ui->btnBlack, ui->btnBlue, ui->btnOrange,
        ui->btnRed, ui->btnPink, ui->btnGreen
    };
    int asize = sizeof(buttons) / sizeof(buttons[0]);

    for(int i = 0; i < asize; i++){
        buttons[i]->setProperty("selected", false);
    }

    switch(color) {
    case HColor::Red:
        ui->btnRed->setProperty("selected", true);
        break;
    case HColor::Orange:
        ui->btnOrange->setProperty("selected", true);
        break;
    case HColor::Blue:
        ui->btnBlue->setProperty("selected", true);
        break;
    case HColor::Black:
        ui->btnBlack->setProperty("selected", true);
        break;
    case HColor::Pink:
        ui->btnPink->setProperty("selected", true);
        break;
    case HColor::Green:
        ui->btnGreen->setProperty("selected", true);
        break;
    }

    for(int i = 0; i < asize; i++) {
        buttons[i]->style()->unpolish(buttons[i]);
        buttons[i]->style()->polish(buttons[i]);
    }

}

void EditImageWindow::scrolled(QGraphicsSceneWheelEvent* evt)
{
    if(evt->delta() > 0 && imageItem->scale() < 4.0) {
        imageItem->setScale(imageItem->scale() + 0.1);
    } else if(evt->delta() < 0 && imageItem->scale() > 0.2) {
        zoom_count--;
        imageItem->setScale(imageItem->scale() - 0.1);
    }
}

void EditImageWindow::textEditMode()
{

}

void EditImageWindow::exitTextEditMode()
{

}


