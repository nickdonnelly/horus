#ifndef IMAGEHELPER_H
#define IMAGEHELPER_H

#include <QObject>
#include <QPixmap>

class ImageHelper : public QObject
{
    Q_OBJECT
public:
    static QPixmap stitch_pixmap(QPixmap * images, int len);

};

#endif // IMAGEHELPER_H
