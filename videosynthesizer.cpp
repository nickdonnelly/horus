#include "videosynthesizer.h"
#include <stdint.h>
#include <QImage>


VideoSynthesizer::VideoSynthesizer(QImage *images, int imageCount)
{
    imageArr = images;
    imageArrSize = imageCount;
}
