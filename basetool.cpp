#include "basetool.h"

BaseTool::BaseTool(QGraphicsScene *scene):
         m_pScene(scene)
{

}
void BaseTool::init()
{

}
void BaseTool::finishEditing()
{
	setEditingInProgress(false);
}
void BaseTool::setEditingInProgress(bool state)
{
	if (m_editing_in_progress != state)
	{
		m_editing_in_progress = state;
	}
}

void BaseTool::mousePositionEvent(QMouseEvent *event ,const QPointF scene_pos)
{
    m_cur_pos =scene_pos;
    if(event->button() == Qt::LeftButton && event->type() == QEvent::MouseButtonPress){
       m_click_pos = m_cur_pos;
    }else if (m_dragging_canceled){
       m_click_pos =m_cur_pos;
       m_dragging_canceled=false;

    }
}
bool BaseTool::mousePressEvent(QMouseEvent* event, const QPointF scene_pos)
{
    mousePositionEvent(event,scene_pos);
    if(event->button()==Qt::LeftButton)
    {
        clickPress();
        return true;
    }else if(event->button()==Qt::RightButton)
    {
        return false;
    }else{
        return false;
    }
}

bool BaseTool::mouseMoveEvent(QMouseEvent* event,  const QPointF scene_pos)
{
     mousePositionEvent(event, scene_pos);

    if (event->buttons().testFlag(Qt::LeftButton))
    {
        if (m_dragging)
        {
            updateDragging();
        }
        else if ((m_cur_pos - m_click_pos).manhattanLength() >= 4)
        {
            startDragging();
        }
        return true;
    }
    else
    {
        mouseMove();
        return false;
    }
}

bool BaseTool::mouseReleaseEvent(QMouseEvent* event,  const QPointF scene_pos)
{
    mousePositionEvent(event,scene_pos);

    if (event->button() == Qt::LeftButton)
    {
        if (m_dragging)
        {
            finishDragging();
        }else
        {
            clickRelease();
        }

        return true;
    }
    else if (event->button() == Qt::RightButton)
    {
        // Do not show the ring menu when editing
        return false;
    }
    else
    {
        return false;
    }
}
void BaseTool::startDragging()
{
    Q_ASSERT(!m_dragging);
    m_dragging = true;
    m_dragging_canceled = false;
    dragStart();
    dragMove();
}

void BaseTool::updateDragging()
{
    Q_ASSERT(m_dragging);
    dragMove();
}

void BaseTool::finishDragging()
{
    Q_ASSERT(m_dragging);
    dragMove();
    m_dragging = false;
    dragFinish();
}
void BaseTool::dragStart()
{

}
void BaseTool::dragMove()
{
    // nothing
}

void BaseTool::dragFinish()
{
    // nothing
}
void BaseTool::mouseMove()
{

}
void BaseTool::clickPress()
{
    // nothing
}

void BaseTool::clickRelease()
{
    // nothing
}
