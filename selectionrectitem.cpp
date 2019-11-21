#include "selectionrectitem.h"
#include <QMatrix4x4>
#include <QGraphicsTransform>
SelectionRectItem::SelectionRectItem(QGraphicsItem *parent)
        :QGraphicsRectItem (parent)
        ,m_group(new QGraphicsItemGroup())
{
  init();
}
void SelectionRectItem::init()
{
    for(int i=0;i<5;i++){
          m_handles[i]=new HandleItem(this);
          m_handles[i]->setTag(i);
          m_handles[i]->setVisible(false);
    }
    this->setPen(QPen(Qt::cyan,1,Qt::DashLine));
    this->setZValue(6);

    m_center = new QGraphicsRectItem(0,0,5,5);
    m_center ->setParentItem(this);
    m_center->setVisible(false);
}
void SelectionRectItem::addToGroup(QGraphicsItem *item)
{
   if(!m_group|!item)
       return;
   m_group->addToGroup(item);
}
void SelectionRectItem::addToGroup(QList<QGraphicsItem*> *itemList)
{
    if(!itemList)
        return;
    if(!m_group)
        return;
    if(m_group->parentItem()==this)
        return;
    for (auto item:*itemList) {
        if(item->isSelected())
            item->setSelected(false);
        addToGroup(item);
    }
    addToSelection(m_group);
}
void SelectionRectItem::removeAllFromGroup()
{
   if(!m_group)
       return;
   removeFromSelection(m_group);
   QList<QGraphicsItem*> itemList= m_group->childItems();
   for(auto item : itemList){
       m_group->removeFromGroup(item);
   }
   //reset m_group
   m_group->resetTransform();
   m_group->setPos(0,0);
   //reset this
   this->resetTransform();
   this->setPos(0,0);
   updateRect();
}
void SelectionRectItem::removeFromGroup(QGraphicsItem *item)
{
    if(!m_group|!item)
        return;
    m_group->removeFromGroup(item);
}
QGraphicsItemGroup * SelectionRectItem::group()
{
   if(!m_group)
       return 0;
   return m_group;
}
void SelectionRectItem::addToSelection(QGraphicsItemGroup *item)
{
    if (!item) {
        qWarning("QGraphicsItemGroup::addToGroup: cannot add null item");
        return;
    }
    if(item==m_group){
        updateRect();
    }
    // COMBINE
    bool ok;
    QTransform itemTransform = item->itemTransform(this, &ok);

    if (!ok) {
        qWarning("QGraphicsItemGroup::addToGroup: could not find a valid transformation from item to group coordinates");
        return;
    }

    QTransform newItemTransform(itemTransform);
    item->setPos(mapFromItem(item, 0, 0));
    item->setParentItem(this);

    // removing position from translation component of the new transform
    if (!item->pos().isNull())
        newItemTransform *= QTransform::fromTranslate(-item->x(), -item->y());

    // removing additional transformations properties applied with itemTransform()
    QPointF origin = item->transformOriginPoint();
    QMatrix4x4 m;
    QList<QGraphicsTransform*> transformList = item->transformations();
    for (int i = 0; i < transformList.size(); ++i)
        transformList.at(i)->applyTo(&m);
    newItemTransform *= m.toTransform().inverted();
    newItemTransform.translate(origin.x(), origin.y());
    newItemTransform.rotate(-item->rotation());
    newItemTransform.scale(1/item->scale(), 1/item->scale());
    newItemTransform.translate(-origin.x(), -origin.y());

    // ### Expensive, we could maybe use dirtySceneTransform bit for optimization

    item->setTransform(newItemTransform);
    //item->d_func()->setIsMemberOfGroup(true);
    prepareGeometryChange();
    //d->itemsBoundingRect |= itemTransform.mapRect(item->boundingRect() | item->childrenBoundingRect());
    update();
}
void SelectionRectItem::removeFromSelection(QGraphicsItemGroup *item)
{

       if (!item) {
           qWarning("QGraphicsItemGroup::removeFromGroup: cannot remove null item");
           return;
       }

       QGraphicsItem *newParent = this->parentItem();

       // COMBINE
       bool ok;
       QTransform itemTransform;
       if (newParent)
           itemTransform = item->itemTransform(newParent, &ok);
       else
           itemTransform = item->sceneTransform();

       QPointF oldPos = item->mapToItem(newParent, 0, 0);
       item->setParentItem(newParent);
       item->setPos(oldPos);

       // removing position from translation component of the new transform
       if (!item->pos().isNull())
           itemTransform *= QTransform::fromTranslate(-item->x(), -item->y());

       // removing additional transformations properties applied
       // with itemTransform() or sceneTransform()
       QPointF origin = item->transformOriginPoint();
       QMatrix4x4 m;
       QList<QGraphicsTransform*> transformList = item->transformations();
       for (int i = 0; i < transformList.size(); ++i)
           transformList.at(i)->applyTo(&m);
       itemTransform *= m.toTransform().inverted();
       itemTransform.translate(origin.x(), origin.y());
       itemTransform.rotate(-item->rotation());
       itemTransform.scale(1 / item->scale(), 1 / item->scale());
       itemTransform.translate(-origin.x(), -origin.y());

       // ### Expensive, we could maybe use dirtySceneTransform bit for optimization

       item->setTransform(itemTransform);
      // item->d_func()->setIsMemberOfGroup(item->group() != 0);

       // ### Quite expensive. But removeFromGroup() isn't called very often.
       prepareGeometryChange();
      // d->itemsBoundingRect = childrenBoundingRect();
}

void SelectionRectItem::updateRect()
{
    if(!m_group)
        return;
  //QPloygonF mapToScene(m_group->boundingRect());
    setRect(m_group->boundingRect());
    updateHandles();

}
void SelectionRectItem::rotateAroundCenter(int angle)
{
    if(!m_group)
        return;

    QTransform groupTransform;
    QPointF center = m_group->boundingRect().center();
    groupTransform.translate(center.x(),center.y());
    groupTransform.rotate(angle);
    groupTransform.translate(-center.x(),-center.y());
    m_group->setTransform(groupTransform);
}
void SelectionRectItem::scaleAroundCenter(qreal x_factor, qreal y_factor)
{
    if(!m_group)
        return;
    QTransform groupTransform;
    QPointF center = m_group->boundingRect().center();
    groupTransform.translate(center.x(),center.y());
    groupTransform.scale(x_factor,y_factor);
    groupTransform.translate(-center.x(),-center.y());
    m_group->setTransform(groupTransform);
    moveGroupPos();
}
void SelectionRectItem::moveGroupPos()
{
    if(!m_group)
        return;
    QPointF parentTopLeft = this->boundingRect().topLeft();
    QPointF groupTopLeft = m_group->boundingRect().topLeft();
    groupTopLeft= m_group->transform().map(groupTopLeft);
    QPointF offset = groupTopLeft-parentTopLeft;
    m_group->setPos(-offset.x(),-offset.y());
}
void SelectionRectItem::rectChanged()
{
     if(!m_group)
        return;
     qreal newWidth = rect().width();
     qreal newHeight = rect().height();
     qreal oldWidth = m_group->boundingRect().width();
     qreal oldHeight = m_group->boundingRect().height();
//     scaleAroundTopLeft(newWidth/oldWidth,newHeight/oldHeight);
     scaleAroundCenter(newWidth/oldWidth,newHeight/oldHeight);
     updateHandles();

}
QPointF SelectionRectItem::rectOrigin()
{
    return m_origin;
}
void SelectionRectItem::updateHandles()
{
    //selection rect center
    m_origin = this->transform().map(this->rect().center());

    QPointF offset(5,5);
    QRectF rect =this->boundingRect();

    qDebug()<<"rect"<<rect;
    for(int i=0;i<5;i++){
         if(!m_handles[i])
             return;
         if(rect.width()<2||rect.height()<2){
                m_handles[i]->setVisible(false);
          }else{
                m_handles[i]->setVisible(true);
          }
             QPointF pos=rect.topLeft();
             switch (i) {
             case HandleItem::TopLeft:
                 pos=rect.topLeft();
                 break;
             case HandleItem::TopRight:
                 pos=rect.topRight();
                 break;
             case HandleItem::BottomRight:
                 pos=rect.bottomRight();
                 break;
             case HandleItem::BottomLeft:
                 pos=rect.bottomLeft();
                 break;
             case HandleItem::Rotate:
                 pos=QPointF(rect.center().x(),rect.top()-rect.height()*0.3);
                 break;
             }
             m_handles[i]->setPos(pos-offset);
             m_handles[i]->setZValue(6);
         }
    if(!m_center)
        return;



    if(rect.width()<2||rect.height()<2){
           m_center->setVisible(false);
     }else{
           m_center->setVisible(true);
     }
     m_center->setPos(this->rect().center()-QPointF(3,3));
}
void SelectionRectItem::scaleAroundTopLeft(qreal x_factor, qreal y_factor){
    if(!m_group)
        return;
    QTransform groupTransform;
    QPointF orgin = rect().topLeft();
    groupTransform.translate(orgin.x(),orgin.y());
    groupTransform.scale(x_factor,y_factor);
    groupTransform.translate(-orgin.x(),-orgin.y());
    m_group->setTransform(groupTransform);
}
void SelectionRectItem::showType()
{
    if(!m_handles[1])
        return;
    qDebug()<<"selection type"<<this->type()<<"handle type "<<m_handles[1]->type();
}
