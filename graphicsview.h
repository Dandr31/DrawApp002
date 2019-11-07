#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H
#include "basetool.h"

#include <QGraphicsView>
QT_BEGIN_NAMESPACE
class QGraphicsSvgItem;
class QSvgRenderer;
class QWheelEvent;
class QPaintEvent;
QT_END_NAMESPACE

class GraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    QTransform transform_set(int l,int n,QTransform transform,qreal value);
    enum RendererType { Native, OpenGL, Image };

    explicit GraphicsView(QWidget *parent = nullptr);

    bool openFile(const QString &fileName);
    bool importSvg(const QString &fileName);
    void drawBackground(QPainter *p, const QRectF &rect) override;
    QSvgRenderer *renderer() const;
    QSize svgSize() const;
    QImage outputBmp();

    //Calculates and returns the bounding rect of all selected items on the scene
    QRectF selectItemsBoundingRect();
    void selectItemAtBox(QRectF rect);
public slots:
     void setViewBackground(bool enable);
     void deleteItem();
     void test();
protected:
    void paintEvent(QPaintEvent *event) override;
private:
    void initDrawTool();
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    BaseTool *m_pTool;
    QGraphicsSvgItem *m_svgItem;
    QGraphicsRectItem *m_backgroundItem;
    QGraphicsRectItem *m_outlineItem;
    QGraphicsScene* m_scene;
    QImage m_image;
    QString path;
    int m_max_import_width=200;
    int m_max_import_height=200;
    QPointF dragStartPoint,dragEndtPoint;

};

#endif // GRAPHICSVIEW_H
