#include "svgtool.h"
#include <QGraphicsSvgItem>
#include <QDebug>
#include <QtMath>
SvgTool::SvgTool(QGraphicsScene *scene):
    BaseTool (scene),
    m_selection(new SelectionRectItem())

{
   Q_ASSERT(m_pScene);
   m_selection_rect= new QGraphicsRectItem(0);
   init();
}

void SvgTool::init()
{
   if(!m_selection|!m_pScene)
       return;
   m_pScene->addItem(m_selection);
}


void SvgTool::dragStart()
{
    updateHoverState(m_click_pos);
	if (m_hover_state == OverNothing)
	{
		m_box_selection = true;
        emptySelection();
//        emptySelectionGroup();
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
       if(m_hover_state==OverSelected){
            if(!m_selection){
                return;
            }
            //m_move_offset Can't be changed when dragging ,so it is set in stratEditing()
            QPointF turePos = m_cur_pos-m_selection->boundingRect().center()-m_move_offset;
            QRectF rect = m_selection->rect();
            rect.moveTo(turePos.x(),turePos.y());

            m_selection->setPos(turePos);
        }
        else if(m_hover_state==OverHandle){
            Q_ASSERT(m_hover_item);
            if(m_hover_item->type()!=HandleItem::Type){
                return;
            }
            if(!m_selection)
                return;
            HandleItem *handle=qgraphicsitem_cast<HandleItem*>(m_hover_item);
            QPointF orgin;
            QRectF rect = m_selection->rect();
            QPointF pos = m_selection->pos();

            QPointF mouse_pos =m_cur_pos;
            int angle=m_selection_angle;
            qreal new_width = rect.width();
            qreal new_height = rect.height();
            qreal old_width =rect.width();
            qreal old_height =rect.height();
            //calculate absolute position of rect
            QTransform a_transform=m_selection->transform();
            QPointF new_topLeft=rect.topLeft(),
                    new_topRight=rect.topRight(),
                    new_bottomRight=rect.bottomRight(),
                    new_bottomLeft=rect.bottomLeft();
            qreal factor;
            qreal min_factor=0.2;
            m_cur_pos= m_selection->mapFromScene(m_cur_pos);
            switch (handle->getTag()) {
                case HandleItem::TopLeft:
                    orgin=rect.bottomRight();
                    new_width=distance_points(m_cur_pos,rect.topRight());
                    new_height=distance_points(m_cur_pos,rect.bottomLeft());

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
                    new_width=distance_points(m_cur_pos,rect.topLeft());
                    new_height=distance_points(m_cur_pos,rect.bottomRight());
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
                    new_width=distance_points(m_cur_pos,rect.bottomLeft());
                    new_height=distance_points(m_cur_pos,rect.topRight());
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
                    new_width=distance_points(m_cur_pos,rect.bottomRight());
                    new_height=distance_points(m_cur_pos,rect.topLeft());

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
                    orgin=m_selection->transform().map(rect.center())+pos;
                    qreal detlaY= mouse_pos.y()-orgin.y();
                    qreal detlaX=mouse_pos.x()-orgin.x();
                    angle = atan2(detlaY,detlaX)*180/M_PI;
                    angle = int(angle+90) ;
                    if ( angle > 360 )
                            angle -= 360;
                    break;
            }


            updateSelection(rect,angle);

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
        updateSelection();
	}
    m_box_selection = false;
}

void SvgTool::updateSelection()
{
    if(!m_pScene)
        return;
    if(!m_selection)
        return;
    QList<QGraphicsItem*> itemList = m_pScene->selectedItems();
    m_selection->addToGroup(&itemList);
    m_selection->showType();
}

void SvgTool:: updateSelection(QRectF rect ,int angle)
{
    if(!m_selection)
        return;

    if(angle!=m_selection_angle){
        QTransform selectionTransform;
        QPointF center = m_selection->rect().center();
        QPointF origin =m_selection->transform().map(center);

       //if you set the rect of m_selection ,the offset will be produced;
        QPointF offset = center-origin;
        selectionTransform.translate(-offset.x(),-offset.y());
        selectionTransform.translate(center.x(),center.y());
        selectionTransform.rotate(angle);
        selectionTransform.translate(-center.x(),-center.y());
        m_selection->setTransform(selectionTransform);
    }else {
        m_selection->setRect(rect);
        m_selection->rectChanged();

    }
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
        m_move_offset =m_click_pos-m_selection->boundingRect().center()-m_selection->pos();
     }
//     m_move_offset=m_click_pos-m_hover_item->boundingRect().center()-m_hover_item->pos();

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
     qDebug()<<"hover item"<<new_hover_item<<new_hover_item->type()<<new_hover_item->zValue();
     if(new_hover_item->type()==HandleItem::Type){
         new_hover_state=OverHandle;
     }
     if(new_hover_item->parentItem()&&new_hover_state!=OverHandle){
         if(new_hover_item->parentItem()->type()==SelectionRectItem::Type){
             new_hover_state=OverSelected;
         }else if(new_hover_item->parentItem()->type()==QGraphicsItemGroup::Type){
             new_hover_state =OverSelected;
         }

     }
     if(new_hover_state!=OverSelected&&new_hover_state!=OverHandle){
         if(new_hover_item->zValue()>-1){
             new_hover_state=OverSingleItem;
         }
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
    qDebug()<<"collidingItems"<<collidingItems;
    if(collidingItems.count()>0){
        for (QGraphicsItem* item : collidingItems)
        {
            if(!item->isVisible())
                continue;
//            if(item->type()==QGraphicsRectItem::Type)
//                continue;
            item->setSelected(true);
        }
    }
    //
    Q_ASSERT(m_selection_rect);
    m_selection_rect->setRect(0,0,0,0);

}


 void SvgTool::emptySelection()
 {
     if(!m_selection)
         return;
     m_selection->removeAllFromGroup();
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
   if(!m_selection)
       return;
   QGraphicsItemGroup *group =m_selection->group();
   if(!group)
       return;
   QList<QGraphicsItem*> itemList=group->childItems();
   m_selection->removeAllFromGroup();
   for(auto item:itemList){
       if(item->scene()){
            item->scene()->removeItem(item);
       }
       delete item;
   }
}
