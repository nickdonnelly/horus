#ifndef HORUSSTYLE_H
#define HORUSSTYLE_H

#include <QProxyStyle>

class HorusStyle : public QProxyStyle
{

public:
    void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const;
};

#endif // HORUSSTYLE_H
