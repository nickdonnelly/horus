#include "horusstyle.h"

void HorusStyle::drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    if(element == QStyle::PE_FrameFocusRect) return; // Don't draw those ugly focus rectangles
    QProxyStyle::drawPrimitive(element, option, painter, widget); // Otherwise do whatever the default is;
}
