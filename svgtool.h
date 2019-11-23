#ifndef SVGTOOL_H
#define SVGTOOL_H
#include "basetool.h"
#include <QGraphicsRectItem>
#include "selectionrectitem.h"
#include "handleitem.h"
class SvgTool:public BaseTool
{
public:
    enum HoverState
        {
            OverNothing      = 0,
            OverSingleItem        = 1,
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


    void drawSelectionRect();

    void updateSelection();

    void updateSelection(QRectF rect ,int angle);

    void emptySelection();

    void updateHoverState(QPointF cur_pos);

    void selectItemAtBox(QRectF rect);

    qreal distance_points(QPointF p1,QPointF p2);

protected:


    bool m_box_selection = false;

    QGraphicsItem *m_hover_item = nullptr;

    HoverState m_hover_state =OverNothing;

    QPointF m_move_offset;

private:

    QGraphicsRectItem *m_selection_rect;

    //m_selection has a child only that is a QGraphicsItemGroup ,
    //it's group has some childItems .So you can change the childeItems by
    //invoking m_selection->setPos(),->setRect(),->setTransform()
    SelectionRectItem *m_selection;



    int m_selection_angle;
};

#endif // SVGTOOL_H
