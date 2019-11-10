#ifndef SVGTOOL_H
#define SVGTOOL_H
#include "basetool.h"
#include <QGraphicsRectItem>
#include "handleitem.h"
class SvgTool:public BaseTool
{
public:
    enum HoverState
        {
            OverNothing      = 0,
            OverFrame        = 1,
            OverHandle       = 3,
            OverSelected     =4
        };

    SvgTool(QGraphicsScene *scene);

    void init() override;

    void dragStart() override;

    void dragMove() override;

    void dragFinish() override;

    void mouseMove() override;

    void startEditing(const QList<QGraphicsItem*> items);

    void clickPress() override;

    void clickRelease() override;

    void deleteSelected();

    void resetSelectionGroup();

    void emptySelectionGroup();

    void drawSelectionRect();

    void updateHandleRect();

    void updateHandleRect(QRectF rect,qreal angle);

    void updateSelectionGroup();

    void updateHoverState(QPointF cur_pos);

    void selectItemAtBox(QRectF rect);

    qreal distance_points(QPointF p1,QPointF p2);

protected:

    HandleItem *m_handles[5];

    bool m_box_selection = false;

    QGraphicsItem *m_hover_item = nullptr;

    HoverState m_hover_state =OverNothing;

    QRectF m_selection_boundingRect;

    QGraphicsItemGroup *m_selection_group;

    QPointF m_move_offset;

    QGraphicsRectItem * m_handle_rect;

private:

    QGraphicsRectItem *m_selection_rect;

    int m_selection_angle;
};

#endif // SVGTOOL_H
