#ifndef IMAGEHELPER_H
#define IMAGEHELPER_H

#include <QObject>
#include <QString>
#include <QPixmap>

class ImageHelper : public QObject
{
    Q_OBJECT
public:
    static QPixmap stitch_pixmap(QPixmap * images, int len, QString filename);

};

#endif // IMAGEHELPER_H
