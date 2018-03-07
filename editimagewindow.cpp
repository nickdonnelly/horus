#include "editimagewindow.h"
#include "screenwindow.h"
#include "ui_editimagewindow.h"
#include <horusuploader.h>
#include <components/horusgraphicsscene.h>
#include <components/horusrectitem.h>
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
#include <QFileDialog>
#include <QStandardPaths>
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
    zoom_count(0),
    uploader(upl),
    fileLoc(filename)
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
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(cancelPressed()));
    connect(ui->actionOpen_Image, SIGNAL(triggered()), this, SLOT(openImage()));
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
    ui->graphicsView->fitInView(imageItem->boundingRect(), Qt::KeepAspectRatio);
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
    connect(rectangleItem,
            SIGNAL(resizeFrom(float,float,HorusRectItem::Corner)),
            this,
            SLOT(rectMouseResize(float,float,HorusRectItem::Corner)));

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

    rectangleItem->setPos(newX, newY);
    outlineItem->setPos(newX, newY);
    enforceRectangleBound();
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
    int rX, rY;
    rX = rectangleItem->rect().x();
    rY = rectangleItem->rect().y();

    ui->lblHeight->setText(QString("Crop Height: ") + QString::number(value));

    rectangleItem->setRect(rX, rY, rectangleItem->rect().width(), value);
    outlineItem->setRect(rX, rY, rectangleItem->rect().width(), value);
    enforceRectangleBound();
}

void EditImageWindow::rectWidthChanged(int value){
    int rX, rY;
    rX = rectangleItem->rect().x();
    rY = rectangleItem->rect().y();

    ui->lblWidth->setText(QString("Crop Width: ") + QString::number(value));

    rectangleItem->setRect(rX, rY, value, rectangleItem->rect().height());
    outlineItem->setRect(rX, rY, value, rectangleItem->rect().height());
    enforceRectangleBound();
}

void EditImageWindow::rectMouseResize(float dx, float dy, HorusRectItem::Corner c)
{
    Q_UNUSED(c)
    float w_scaled = imageItem->scale() * imgOriginalWidth;
    float h_scaled = imageItem->scale() * imgOriginalHeight;
    QRectF r = rectangleItem->rect();
    if(rectangleItem->x() + (r.width() - dx) <= imageItem->x() + w_scaled) {
        r.setWidth(std::max(1, (int)(r.width() - dx)));
    }

    if(rectangleItem->y() + (r.height() - dy) <= imageItem->y() + h_scaled) {
        r.setHeight(std::max(1, (int)(r.height() - dy)));
    }

    rectangleItem->setRect(r);
    outlineItem->setRect(r);

    enforceRectangleBound();
}

void EditImageWindow::rectMouseDown(QPointF position)
{
    startX = position.x();
    startY = position.y();
    dragging = true;
}

void EditImageWindow::rectMouseUp()
{
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
    if(!inDrawingMode){
        float dx = position.x() - startPanX;
        float dy = position.y() - startPanY;
        QPointF currentPos = imageItem->pos();
        float newX = (float) currentPos.x() + dx;
        float newY = (float) currentPos.y() + dy;

        imageItem->setPos(newX, newY);
        enforceRectangleBound();
    }
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
    delete croppedPainter;
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
    imageItem->setScale(1.0);
    scene->addItem(imageItem);
    scene->addItem(rectangleItem);
    scene->addItem(outlineItem);
    ui->graphicsView->fitInView(imageItem->boundingRect(), Qt::IgnoreAspectRatio);
    ui->graphicsView->centerOn(imageItem);
    imageItem->setScale(1);
    imageItem->setPos(0, 0);

    rectangleItem->setPos(0, 0);
    outlineItem->setPos(0, 0);

    rectangleItem->setVisible(true);
    outlineItem->setVisible(true);

    imageItem->setZValue(1);
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
    if(evt->delta() > 0 && imageItem->scale() < 5.0) {
        imageItem->setScale(imageItem->scale() + 0.1);
    } else if(evt->delta() < 0 && imageItem->scale() > 0.2) {
        zoom_count--;
        imageItem->setScale(imageItem->scale() - 0.1);
    }

    // Check our rectangle is still in bounds.
    enforceRectangleBound();
}

void EditImageWindow::enforceRectangleBound() {
    QPointF rectPos = rectangleItem->pos();
    QPointF imagePos = imageItem->pos();
    float w_scaled = imgOriginalWidth * imageItem->scale();
    float h_scaled = imgOriginalHeight * imageItem->scale();

    float newX = rectPos.x();
    float newY = rectPos.y();

    // Width
    if(rectangleItem->rect().width() > w_scaled){
        rectangleItem->rect().setWidth(w_scaled);
        ui->sliderW->setValue(w_scaled);
    } else {
        ui->sliderW->setValue(rectangleItem->rect().width());
    }

    // Height
    if(rectangleItem->rect().height() > h_scaled){
        rectangleItem->rect().setHeight(h_scaled);
        ui->sliderH->setValue(h_scaled);
    } else {
        ui->sliderH->setValue(rectangleItem->rect().height());
    }

    // Left Edge
    if(rectPos.x() < imagePos.x()){
        newX = imagePos.x();
    }

    // Top Edge
    if(rectPos.y() < imagePos.y()){
        newY = imagePos.y();
    }

    // Right Edge
    if(rectPos.x() + rectangleItem->rect().width() > imagePos.x() + w_scaled) {
        float diff = (rectPos.x() + rectangleItem->rect().width()) - (imagePos.x() + w_scaled);
        newX = newX - diff;
    }

    // Bottom Edge
    if(rectPos.y() + rectangleItem->rect().height() > imagePos.y() + h_scaled) {
        float diff = (rectPos.y() + rectangleItem->rect().height()) - (imagePos.y() + h_scaled);
        newY = newY - diff;
    }

    rectangleItem->setPos(newX, newY);
    outlineItem->setPos(newX, newY);
}

void EditImageWindow::openImage()
{
    // open find window.
    QString file = QFileDialog::getOpenFileName(this,
                    "Select an image to open...",
                    QStandardPaths::writableLocation(QStandardPaths::PicturesLocation),
                    "Images (*.png *.jpg *.jpeg)");

    if(file != "") {
        fileLoc = file;
        scene->removeItem(imageItem);
        imageItem->deleteLater();
        delete imagePixmap;
        imagePixmap = new QPixmap(fileLoc);
        imgOriginalWidth = imagePixmap->width();
        imgOriginalHeight = imagePixmap->height();

        rectangleItem->rect().setWidth(imgOriginalWidth * 0.15);
        rectangleItem->rect().setHeight(imgOriginalHeight * 0.15);

        ui->sliderW->setMaximum(imgOriginalWidth);
        ui->sliderW->setMinimum(1);
        ui->sliderH->setMaximum(imgOriginalHeight);
        ui->sliderH->setMinimum(1);

        imageItem = new HorusPixmapGraphicsItem(*imagePixmap);
        scene->addItem(imageItem);

        ui->graphicsView->centerOn(imageItem);
        showingCropped = false;

        connect(imageItem, SIGNAL(lMouseDown(QPointF)), this, SLOT(panStart(QPointF)));
        connect(imageItem, SIGNAL(lMouseUp()), this, SLOT(panEnd()));
        connect(imageItem, SIGNAL(mouseMoved(QPointF)), this, SLOT(panMove(QPointF)));

        enforceRectangleBound();

    }
}
