#include "imagehelper.h"
#include <QPixmap>
#include <QPainter>

QPixmap ImageHelper::stitch_pixmap(QPixmap * images, int len){

    int totalW = 0;
    int maxH = 0;
    // First iteration to get the dimensions
    for(int i = 0; i < len; i++){
        maxH = std::max(maxH, images[i].height());
        totalW += images[i].width();
    }
    // Set the dimensions
    QPixmap result(totalW, maxH);
    QPainter *painter = new QPainter(&result);
    int curX = 0;
    // Second iteration to paint the pixmap.
    for(int i = 0; i < len; i++){
        painter->drawPixmap(curX, 0, images[i].width(), images[i].height(), images[i]);
        curX += images[i].width();
    }
    result.save("abc123.png");

    return result;
}
