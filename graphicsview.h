#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H
#include "basetool.h"
#include <QPainter>
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
    enum RendererType { Native, OpenGL, Image };

    explicit GraphicsView(QWidget *parent = nullptr);

    bool openFile(const QString &fileName);

    bool importSvg(const QString &fileName);

    bool importImage(const QString &fileName);


    QSvgRenderer *renderer() const;

    QSize svgSize() const;

    QImage outPutImage();

    bool isSceneEmpty();

    bool exprotSvg(const QString &fileName);

    bool exportGcode(const QString &fileName);

public slots:
     void setViewBackground(bool enable);

     void deleteItem();

     void test();

     void test2();


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

    //effective robot's coordinate rect
    QRect m_working_rect;

    QGraphicsRectItem *m_workingItem;

    QGraphicsLineItem *m_xLineItem;

    int m_background_zValue = -1;

    int m_max_import_width=200;

    int m_max_import_height=200;


};

#endif // GRAPHICSVIEW_H
