#ifndef VIDEOSYNTHESIZER_H
#define VIDEOSYNTHESIZER_H

#include <QObject>
#include <QImage>

class VideoSynthesizer:public QObject
{
    Q_OBJECT
public:
    VideoSynthesizer(QImage *images, int imageCount);

private:
    QImage * imageArr;

    int imageArrSize;
};

#endif // VIDEOSYNTHESIZER_H
