#ifndef HANDLEITEM_H
#define HANDLEITEM_H
#include <QGraphicsItem>
#include <QPainter>
class HandleItem:public QGraphicsItem
{
public:
    enum HandleTag{TopRight,TopLeft,BottomLeft,BottomRight,Rotate};
    enum { Type = UserType + 1 };
    HandleItem(QGraphicsItem * parent=0);
    void setTag(int tag);
    int getTag();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    QRectF boundingRect() const override;
    int type() const override
      {
               // Enable the use of qgraphicsitem_cast with this item.
               return Type;
     }
private:
    int m_tag = Rotate;
    int m_width ;
    int m_height;
};

#endif // HANDLEITEM_H
