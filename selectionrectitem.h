#ifndef SELECTIONRECTITEM_H
#define SELECTIONRECTITEM_H
#include <QGraphicsRectItem>
#include <QGraphicsItemGroup>
#include <QGraphicsRectItem>
#include "handleitem.h"
class SelectionRectItem:public QGraphicsRectItem
{
public:
    SelectionRectItem(QGraphicsItem *parent=0);
    enum { Type = UserType + 2 };
    void init();

    QGraphicsItemGroup * group();

    void addToGroup(QGraphicsItem *item);

    void addToGroup(QList<QGraphicsItem*> *itemList);

    void removeFromGroup(QGraphicsItem *item);

    void removeAllFromGroup();

    void addToSelection(QGraphicsItemGroup *item);

    void removeFromSelection(QGraphicsItemGroup *item);

    void rotateAroundCenter(int angle);

    void moveGroupPos();

    void rectChanged();

    QPointF rectOrigin();

    void updateHandles();

    void scaleAroundCenter(qreal x_factor,qreal y_factor);

    void scaleAroundTopLeft(qreal x_factor,qreal y_factor);

    void showType();
    int type() const override
      {
               // Enable the use of qgraphicsitem_cast with this item.
               return Type;
     }
private:
    void updateRect();
private:
    HandleItem *m_handles[5];

    QGraphicsRectItem *m_center;

    QPointF m_origin;

    QGraphicsItemGroup *m_group;
};

#endif // SELECTIONRECTITEM_H
