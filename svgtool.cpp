#include "svgtool.h"
#include <QGraphicsSvgItem>
#include <QDebug>
#include <QtMath>
SvgTool::SvgTool(QGraphicsScene *scene):
    BaseTool (scene)
{
   Q_ASSERT(m_pScene);
  m_selection_rect= new QGraphicsRectItem(0);
  m_selection_group = new QGraphicsItemGroup();
  m_handle_rect=new QGraphicsRectItem(0);
  init();
}

void SvgTool::init()
{
    if(!m_handle_rect)
        return;
    for(int i=0;i<5;i++){
        m_handles[i]=new HandleItem(m_handle_rect);
        m_handles[i]->setTag(i);
    }
}


void SvgTool::dragStart()
{
    updateHoverState(m_click_pos);
	if (m_hover_state == OverNothing)
	{
		m_box_selection = true;
        emptySelectionGroup();
    }
    else
	{
        startEditing(m_pScene->selectedItems());
	}
}

void SvgTool::dragMove()
{
     Q_ASSERT(m_pScene);
    if (editingInProgress())
	{
//     qDebug()<<"dragMove"<<m_hover_state<<m_hover_item<<"type"<<m_hover_item->type()<<HandleItem::Type;
        if(m_hover_state==OverFrame){
            Q_ASSERT(m_hover_item);
     //m_move_offset Can't be changed when dragging ,so it is set in stratEditing()
            QPointF turePos = m_cur_pos-m_hover_item->boundingRect().center()-m_move_offset;
            m_hover_item->setPos(turePos);
//            updateHandleRect();
            resetSelectionGroup();
        }else if(m_hover_state==OverSelected){
            Q_ASSERT(m_hover_item);
            QPointF turePos = m_cur_pos-m_hover_item->boundingRect().center()-m_move_offset;
            m_hover_item->setPos(turePos);
//             updateHandleRect();
            resetSelectionGroup();
        }
        else if(m_hover_state==OverHandle){
            Q_ASSERT(m_hover_item);
            if(!m_selection_group){
                return;
            }
            if(m_hover_item->type()!=HandleItem::Type){
                return;
            }
            HandleItem *handle=qgraphicsitem_cast<HandleItem*>(m_hover_item);
            QPointF orgin;
            QRectF rect=m_handle_rect->rect();
            QPointF pos = m_selection_group->pos();

            QPointF mouse_pos =m_cur_pos;
            int angle=m_selection_angle;
            qreal new_width = rect.width();
            qreal new_height = rect.height();
            qreal old_width =rect.width();
            qreal old_height =rect.height();
            //calculate absolute position of rect
            QTransform a_transform=m_handle_rect->transform();
            QPointF abs_tl=a_transform.map(rect.topLeft());
            QPointF abs_tr=a_transform.map(rect.topRight());
            QPointF abs_br=a_transform.map(rect.bottomRight());
            QPointF abs_bl=a_transform.map(rect.bottomLeft());

            QPointF new_topLeft=rect.topLeft(),
                    new_topRight=rect.topRight(),
                    new_bottomRight=rect.bottomRight(),
                    new_bottomLeft=rect.bottomLeft();
            qreal factor;
            qreal min_factor=0.2;
            switch (handle->getTag()) {
                case HandleItem::TopLeft:
                    orgin=rect.bottomRight();
                    new_width=distance_points(m_cur_pos,abs_tr);
                    new_height=distance_points(m_cur_pos,abs_bl);

                    factor =qMin(new_width/old_width,new_height/old_height);
                    if(factor<min_factor)
                          factor=min_factor;
                    new_width=old_width*factor;
                    new_height=old_height*factor;

                    new_topLeft.setX(new_bottomRight.x()-new_width);
                    new_topLeft.setY(new_bottomRight.y()-new_height);
                    rect.setTopLeft(new_topLeft);
                    rect.setWidth(new_width);
                    rect.setHeight(new_height);
                    break;
                case HandleItem::TopRight:
                    orgin=rect.bottomLeft();
                    new_width=distance_points(m_cur_pos,abs_tl);
                    new_height=distance_points(m_cur_pos,abs_br);
                    factor =qMin(new_width/old_width,new_height/old_height);
                    if(factor<min_factor)
                          factor=min_factor;
                    new_width=old_width*factor;
                    new_height=old_height*factor;
                    new_topRight.setX(new_bottomLeft.x()+new_width);
                    new_topRight.setY(new_bottomLeft.y()-new_height);
                    rect.setTopRight(new_topRight);

                    break;
                case HandleItem::BottomRight:
                    orgin=rect.topLeft();
                    new_width=distance_points(m_cur_pos,abs_bl);
                    new_height=distance_points(m_cur_pos,abs_tr);
                    factor =qMin(new_width/old_width,new_height/old_height);
                    if(factor<min_factor)
                          factor=min_factor;
                    new_width=old_width*factor;
                    new_height=old_height*factor;
                    new_bottomRight.setX(new_topLeft.x()+new_width);
                    new_bottomRight.setY(new_topLeft.y()+new_height);
                    rect.setBottomRight(new_bottomRight);

                    break;
                case HandleItem::BottomLeft:
                    orgin=rect.topRight();
                    new_width=distance_points(m_cur_pos,abs_br);
                    new_height=distance_points(m_cur_pos,abs_tl);

                    factor =qMin(new_width/old_width,new_height/old_height);
                    if(factor<min_factor)
                          factor=min_factor;
                    new_width=old_width*factor;
                    new_height=old_height*factor;

                    new_bottomLeft.setX(new_topRight.x()-new_width);
                    new_bottomLeft.setY(new_topRight.y()+new_height);
                    rect.setBottomLeft(new_bottomLeft);

                    break;
                case HandleItem::Rotate:
                    orgin=rect.center()+pos;
                    qreal detlaY= mouse_pos.y()-orgin.y();
                    qreal detlaX=mouse_pos.x()-orgin.x();
                    angle = atan2(detlaY,detlaX)*180/M_PI;
                    angle = int(angle+90) ;
                    if ( angle > 360 )
                            angle -= 360;
                    break;
            }

            updateHandleRect(rect,angle);

           }

      } else if (m_box_selection)
      {
        drawSelectionRect();
      }
   

}


void SvgTool::dragFinish()
{
    if (editingInProgress())
	{
		finishEditing();
	}
    else if (m_box_selection)
	{
        selectItemAtBox(m_selection_rect->boundingRect());
        updateSelectionGroup();
	}
    m_box_selection = false;
}


void SvgTool::drawSelectionRect()
{
    QRectF rect;
    rect.setTopLeft(m_click_pos);
    rect.setBottomRight(m_cur_pos);
    m_selection_rect->setRect(rect);
    Q_ASSERT(m_pScene);
    if(!m_selection_rect->scene()){
        m_pScene->addItem(m_selection_rect);
    }
}

void SvgTool::mouseMove()
{
    Q_ASSERT(m_pScene);

  
}

//todo : select single item by clickpress
void SvgTool::clickPress()
{
  qDebug()<<"cur_pos"<<m_cur_pos<<"click press";
  updateHoverState(m_cur_pos);
}

void SvgTool::clickRelease()
{

}

void SvgTool::startEditing(const QList<QGraphicsItem*> items)
{
     Q_UNUSED(items);
     Q_ASSERT(!editingInProgress());
     Q_ASSERT(m_hover_item);
     if(m_hover_state==OverSelected){
         if(m_hover_item->parentItem())
             m_hover_item=m_hover_item->parentItem();
     }
     m_move_offset=m_click_pos-m_hover_item->boundingRect().center()-m_hover_item->pos();

	 setEditingInProgress(true);

 }


 void SvgTool::updateHoverState(QPointF cur_pos)
 {
     Q_ASSERT(m_pScene);
     QTransform deviceTransform;
     HoverState new_hover_state =OverNothing;
     QGraphicsItem* new_hover_item = nullptr;
     new_hover_item=m_pScene->itemAt(cur_pos,deviceTransform);
     if(new_hover_item==0){
         new_hover_state=OverNothing;
         m_hover_state=OverNothing;
         qDebug()<<"HoverNothing"<<m_hover_state;
         return;
     }
     qDebug()<<"hover item"<<new_hover_item;
     if(new_hover_item->type()==HandleItem::Type){
         new_hover_state=OverHandle;
     }
     if(new_hover_item->type()==QGraphicsItemGroup::Type){
         new_hover_state=OverFrame;
     }
     if(new_hover_item->parentItem()){
         if(new_hover_item->parentItem()->type()==QGraphicsItemGroup::Type)
             new_hover_state=OverSelected;
     }
     if (new_hover_state  != m_hover_state  ||
        new_hover_item   != m_hover_item)
     {
		m_hover_state = new_hover_state;
        m_hover_item = new_hover_item;
     }
    qDebug()<<"HoverState"<<m_hover_state<<m_hover_item<<"type"<<m_hover_item->type();
 }


 void SvgTool::selectItemAtBox(QRectF rect)
{
    if(!m_pScene||!m_selection_rect){
        return;
    }
    QList<QGraphicsItem*> collidingItems=m_pScene->collidingItems(m_selection_rect);
    if(collidingItems.count()>0){
        for (QGraphicsItem* item : collidingItems)
        {
            if(!item->isVisible())
                continue;
            if(item->type()!=QGraphicsRectItem::Type)
                continue;
            item->setSelected(true);
        }
    }
    //
    Q_ASSERT(m_selection_rect);
    m_selection_rect->setRect(0,0,0,0);

}


void SvgTool::updateSelectionGroup()
{
    if(!m_pScene)
        return;
    if(!m_selection_group)
        return;
    QList<QGraphicsItem*> items = m_pScene->selectedItems();
//    if(items.count()==1){
//        m_selection_group->setTransform(items.first()->transform());
//    }
    for(QGraphicsItem *item:items){
        item->setSelected(false);
        m_selection_group->addToGroup(item);
    }
    if(!m_selection_group->scene())
        m_selection_group->setZValue(2);
        m_pScene->addItem(m_selection_group);
    updateHandleRect();

}

void SvgTool::emptySelectionGroup()
{
     qDebug()<<"m_selection_group "<<m_selection_group<<m_selection_group->boundingRect();
    if(!m_pScene||!m_selection_group)
         return;
    if(m_selection_group->scene()==m_pScene){
        for(QGraphicsItem* item:m_selection_group->childItems()){
            qDebug()<<"children items"<<item;
            m_selection_group->removeFromGroup(item);
        }
        m_selection_group->resetTransform();
        m_pScene->removeItem(m_selection_group);
        m_selection_group->setPos(0,0);
    }
    m_selection_angle=0;
    updateHandleRect();
    qDebug()<<"m_selection_group remove"<<m_selection_group<<m_selection_group->boundingRect();


}


void SvgTool::updateHandleRect()
{
    if(!m_selection_group||!m_handle_rect){
        return;
    }

    m_handle_rect->setRect(m_selection_group->boundingRect());
    m_handle_rect->setPos(m_selection_group->pos());
    QTransform transform=m_selection_group->transform();
    m_handle_rect->setTransform(transform);
    QPointF offset(3,3);
    QRectF rect =m_handle_rect->boundingRect();
    for(int i=0;i<5;i++){
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
            pos=QPointF(rect.center().x(),rect.top()-70);
            break;
        }
        m_handles[i]->setPos(pos-offset);
        m_handles[i]->setZValue(6);
    }
//    qDebug()<<m_selection_rect<<m_selection_rect->boundingRect();
    if(!m_handle_rect->scene()){
        if(!m_pScene)
            return;
//        m_handle_rect->setZValue(2);
        m_pScene->addItem(m_handle_rect);
    }

}


void SvgTool::updateHandleRect(QRectF t_rect,qreal angle){

     if(!m_selection_rect||!m_handle_rect)
          return;
      QRectF b_rect = m_selection_group->boundingRect();
      //hanlde_rect


      m_handle_rect->setRect(t_rect);
      QTransform r_transform;
      QPointF r_center=m_handle_rect->rect().center()+m_handle_rect->pos();
      r_transform.translate(r_center.x(),r_center.y());
      r_transform.rotate(angle);
      r_transform.translate(-r_center.x(),-r_center.y());
      if(angle!=m_selection_angle){
           m_handle_rect->setTransform(r_transform);
      }

      //selection_group
      QTransform transform;
      QPointF center = b_rect.center();
      qreal x_factor = t_rect.width()/b_rect.width();
      qreal y_factor = t_rect.height()/b_rect.height();
      if(x_factor<y_factor){
          y_factor=x_factor;
      }else if(x_factor>y_factor){
          x_factor=y_factor;
      }
      transform.translate(center.x(),center.y());
      transform.rotate(angle);
      transform.translate(-center.x(),-center.y());
      transform.translate(center.x(),center.y());
      transform.scale(x_factor,y_factor);
      transform.translate(-center.x(),-center.y());
      m_selection_group->setTransform(transform);
      QPointF old_pos = m_selection_group->pos();
      QPointF new_pos;
      QPointF new_center = m_selection_group->transform().map(m_selection_group->boundingRect().center());
//    QPointF t_topLeft =r_transform.map(t_rect.topLeft());
      QPointF t_center = m_handle_rect->transform().map(m_handle_rect->rect().center());
      new_pos =new_center-t_center;
      m_selection_group->setPos(-new_pos.x(),-new_pos.y());

      m_selection_angle=angle;
      QPointF offset(3,3);
      QRectF rect =m_handle_rect->boundingRect();
      for(int i=0;i<5;i++){
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
              pos=QPointF(rect.center().x(),rect.top()-70);
              break;
          }
          m_handles[i]->setPos(pos-offset);
          m_handles[i]->setZValue(6);
      }


}

//return the distance between two points
qreal SvgTool::distance_points(QPointF p1,QPointF p2)
{
    qreal detla_x = p1.x()-p2.x();
    qreal detla_y = p1.y()-p2.y();
    qreal distance = qSqrt(qPow(detla_x,2)+qPow(detla_y,2));
    return distance;
}

void SvgTool::deleteSelected()
{
    if(!m_selection_group){
        return;
    }
    QList<QGraphicsItem*> items = m_selection_group->childItems();
    emptySelectionGroup();
    updateHandleRect();
    for(QGraphicsItem*item:items){
        if(!item->scene()){
            continue;
        }
        m_pScene->removeItem(item);
    }
}
//being used when move items
 void SvgTool::resetSelectionGroup()
 {
     if(!m_selection_group)
         return;
     QList<QGraphicsItem*> items = m_selection_group->childItems();

     emptySelectionGroup();

     for(QGraphicsItem* item:items){
         m_selection_group->addToGroup(item);
     }
     if(!m_selection_group->scene()){
         if(!m_pScene)
             return;
         m_pScene->addItem(m_selection_group);
     }
     updateHandleRect();
 }
