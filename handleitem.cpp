#include "handleitem.h"

HandleItem::HandleItem(QGraphicsItem *parent):
    QGraphicsItem (parent)
{
    m_width=10;
    m_height=10;

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
     painter->drawRect(0,0,m_width,m_height);
 }
QRectF HandleItem::boundingRect() const
{
    qreal adjust = 0;
    return QRectF( 0-adjust, 0-adjust, m_width +adjust, m_height+adjust);
}
