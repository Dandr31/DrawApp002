#ifndef BASETOOL_H
#define BASETOOL_H


#include <QGraphicsScene>
#include <QMouseEvent>
#include <QCursor>
class  BaseTool
{
public:
    BaseTool(QGraphicsScene *scene);
//    ~BaseTool();

    //Initialization when the tool becomes active;
    virtual void init();


    //Mouse input
    void mousePositionEvent(QMouseEvent* event, const QPointF scene_position);

    bool mousePressEvent(QMouseEvent *event,const QPointF scene_position);
    bool mouseMoveEvent(QMouseEvent *event,const QPointF scene_position);
    bool mouseReleaseEvent(QMouseEvent *event,const QPointF scene_position);

    
    bool editingInProgress() const { return m_editing_in_progress; }

    virtual void finishEditing();
    
    void startDragging();
    void updateDragging();
    void finishDragging();

    virtual void clickPress();
    virtual void clickRelease();

    virtual void mouseMove();
    virtual void dragStart();
    virtual void dragMove();
    virtual void dragFinish();

    QGraphicsScene *scene() const;
protected:
     void setEditingInProgress(bool state); 

     QPointF m_click_pos;

     QPointF m_cur_pos;

     QGraphicsScene *m_pScene;
private:
    QCursor cursor;

    bool m_dragging                 = false;

    bool m_dragging_canceled        = false;

    bool m_editing_in_progress      = false;


};
#endif // BASETOOL_H
