#include "handleitem.h"

HandleItem::HandleItem(QGraphicsItem *parent):
    QGraphicsItem (parent)
{

}
void HandleItem::setTag(int tag){
    if(m_tag!=tag){
        m_tag=tag;
    }
}
int HandleItem::getTag()
{
    return m_tag;
}
 void HandleItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
 {
     Q_UNUSED(option);
     Q_UNUSED(widget);
     painter->setPen(QPen(Qt::blue,1));
     painter->drawRect(0,0,8,8);
 }
QRectF HandleItem::boundingRect() const
{
    qreal adjust = 2;
    return QRectF( 0-adjust, 0-adjust, 8 +adjust, 8+adjust);
}
