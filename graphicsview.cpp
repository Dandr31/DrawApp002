#include "graphicsview.h"
#include <QDebug>

#include <QSvgRenderer>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QGraphicsRectItem>
#include <QGraphicsSvgItem>
#include <QPaintEvent>
#include <qmath.h>
#include <QSvgGenerator>
#include <QBuffer>
#include <QFileDialog>
#include <QDateTime>
#include <QProcess>
#include <QCoreApplication>
#include "util.h"
#include "svgtool.h"
#include "tracer.h"
GraphicsView::GraphicsView(QWidget *parent)
: QGraphicsView(parent)
 , m_svgItem(nullptr)
 , m_backgroundItem(nullptr)
 , m_workingItem(nullptr)

{
    setScene(new QGraphicsScene(this));

    setTransformationAnchor(AnchorUnderMouse);

   // setDragMode(ScrollHandDrag);

    setViewportUpdateMode(FullViewportUpdate);

     m_scene =scene();

     initDrawTool();

}
void GraphicsView::initDrawTool()
{
    scene()->clear();
    resetTransform();
    scene()->setSceneRect(0,0,700,300);

    int padding = 20;
    QRect paddingRect(padding,padding,scene()->width()-2*padding,scene()->height()-2*padding);
    // working space
    qreal robot_w = 640;
    qreal robot_h = 160;
    QPointF robot_orgin(paddingRect.center().x(),paddingRect.bottom()-220);
    m_working_rect =QRect(robot_orgin.x()-robot_h*2,robot_orgin.y(),robot_w,robot_h);
    m_robot_rect = QRect(m_working_rect.x(),m_working_rect.y(),robot_w,220);
    m_workingItem = new QGraphicsRectItem(m_working_rect);
    m_workingItem->setPen(QPen(Qt::cyan,1,Qt::DashLine));
    m_workingItem->setBrush(Qt::NoBrush);
    m_workingItem->setVisible(true);
    m_workingItem->setZValue(m_background_zValue);
    QGraphicsRectItem *paddingItem = new QGraphicsRectItem(paddingRect);
    paddingItem->setPen(QPen(Qt::black,1,Qt::DashLine));
    paddingItem->setZValue(m_background_zValue);
    //coordinate x and y line
    m_xLineItem = new QGraphicsLineItem(paddingRect.center().x(),paddingRect.bottom(),paddingRect.center().x(),m_working_rect.bottom());
    m_xLineItem->setPen(QPen(Qt::gray,1));
    m_xLineItem->setZValue(m_background_zValue);
    QGraphicsRectItem *robotItem = new QGraphicsRectItem(m_robot_rect);
    robotItem->setPen(QPen(Qt::yellow,1));
    robotItem->setZValue(m_background_zValue);

    QImage new_image("F://start//QT//res//miku.jpg");
    QGraphicsPixmapItem * n_pixmap=new QGraphicsPixmapItem();
    QPixmap tempPixmap = QPixmap::fromImage(new_image);
    n_pixmap->setPixmap(tempPixmap);
    n_pixmap->setScale(0.1);
    n_pixmap->setPos(0,0);

//    QTransform transform2;
//    transform2.translate(center.x(),center.y());
//    transform2.scale(2,3);
//    transform2.translate(-center.x(),-center.y());
//    transform*=transform;
//    scene()->addItem(n_pixmap);

//    scene()->addItem(m_backgroundItem);
    scene()->addItem(paddingItem);
    scene()->addItem(m_xLineItem);
    scene()->addItem(robotItem);
    scene()->addItem(m_workingItem);
    m_pTool = new SvgTool(scene());

}

QSize GraphicsView::svgSize() const
{
    return m_svgItem ? m_svgItem->boundingRect().size().toSize() : QSize();
}
bool GraphicsView::openFile(const QString &fileName)
{
    QString suffix =getSuffix(fileName);
    if(suffix=="svg"){
       return importSvg(fileName);
    }else if(suffix=="jpg"||suffix=="png"){
//        if(!scene())
//            return false;
//         QPixmap pixmap = QPixmap::fromImage(QImage(fileName));
//         QGraphicsPixmapItem *pix =new QGraphicsPixmapItem(pixmap);
//         pix->setFlag(QGraphicsItem::ItemIsSelectable);
//         pix->setScale(0.1);
//         scene()->addItem(pix);

       return importImage(fileName);
    }
    /*
    QScopedPointer<QGraphicsSvgItem> svgItem(new QGraphicsSvgItem(fileName));
    if (!svgItem->renderer()->isValid())
        return false;
    m_svgItem = svgItem.take();
    m_svgItem->setFlags(QGraphicsItem::ItemClipsToShape);
    m_svgItem->setCacheMode(QGraphicsItem::NoCache);
    m_svgItem->setZValue(0);
    m_svgItem->setFlag(QGraphicsItem::ItemIsSelectable);
    s->addItem(m_svgItem);
    */
    return false;
}

void GraphicsView::paintEvent(QPaintEvent *event)
{
    QGraphicsView::paintEvent(event);
}

void GraphicsView::setViewBackground(bool enable)
{
    if (!m_backgroundItem)
          return;

    m_backgroundItem->setVisible(enable);
}
void GraphicsView::deleteItem(){
    if(!m_pTool){
        return;
    }
    SvgTool *svgTool = static_cast<SvgTool*>(m_pTool);
    svgTool->deleteSelected();
}

void GraphicsView::mousePressEvent(QMouseEvent *event)
{
//      qDebug()<<"event view pos"<<event->pos();

    if (m_pTool && m_pTool->mousePressEvent(event,mapToScene( event->pos())))
         {

         }
    update();
    QGraphicsView::mousePressEvent(event);//让事件传递

}

void GraphicsView::mouseMoveEvent(QMouseEvent *event)
{

    if (m_pTool && m_pTool->mouseMoveEvent(event,mapToScene( event->pos())))
        {

        }
    update();
    QGraphicsView::mouseMoveEvent(event);
}
void GraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_pTool && m_pTool->mouseReleaseEvent(event, mapToScene( event->pos())))
        {
        }
    update();
    QGraphicsView::mouseReleaseEvent(event);
}
QImage GraphicsView::outPutImage()
{
    //bug:cant render correct image when use  release
    //but debug can render

    if(m_pTool){
        SvgTool *svgTool = static_cast<SvgTool*>(m_pTool);
        svgTool->emptySelection();
    }
    if(m_xLineItem){
        m_xLineItem->setVisible(false);
    }
    if(m_workingItem){
        m_workingItem->setVisible(false);
    }
    if(isSceneEmpty()){
        QImage emptyImage;
        return emptyImage;
    }

    QImage image(scene()->width(),scene()->height(),QImage::Format_Grayscale8);

    image.fill(Qt::white);//if not use it ,will create black picture in release

    QPainter painter;

    painter.begin(&image);

    m_scene->clearSelection();

    m_scene->render(&painter);

    painter.end();

    image = image.copy( m_robot_rect.adjusted(1,1,-1,-1));

    if(m_xLineItem){
        m_xLineItem->setVisible(true);
    }
    if(m_workingItem){
        m_workingItem->setVisible(true);
    }
    return image;

}
void GraphicsView::test2()
{

}
void GraphicsView::test()
{


}

bool GraphicsView::importImage(const QString &fileName)
{

    Tracer * tracer = new Tracer();
    if(!tracer)
          return false;
    /*Hancock*/
//    QString target_name = "haokan.svg";
//    QImage *pImage =new QImage("F:\\start\\QT\\res\\haokan.png");
//    QString target = "F:\\start\\QT\\selectionGroup\\res\\"+target_name;
    QImage *pImage = new QImage(fileName);
    QString svgPath = replaceSuffix(fileName,"svg");
    if(tracer->traceToSvg(pImage,svgPath)){
        if(importSvg(svgPath)){
            QFile file(svgPath);
            if(file.exists())
                file.remove();
            return true;
        }
    }
    return false;
}

bool GraphicsView::importSvg(const QString &fileName)
{
        QSize size(m_max_import_width,m_max_import_height);

        QSvgRenderer *newRenderer = process_svgResizeTo(fileName,size);

        if(!newRenderer)
            return false;

        QGraphicsSvgItem *new_svg = new QGraphicsSvgItem();

        new_svg->setSharedRenderer(newRenderer);

        if(!new_svg)
            return false;
        if(!scene()){
            return false;
        }
        new_svg->setFlag(QGraphicsItem::ItemIsSelectable);

        scene()->addItem(new_svg);

        qDebug()<<"import miku boundingRect"<<new_svg->boundingRect();
        return true;
}

bool GraphicsView::exportGcode(const QString &fileName)
{
     Tracer *tracer=new Tracer();
     if(!tracer)
         return false;
     QImage image = outPutImage();
     if(image.width()==0||image.height()==0)
         return false;
     image.save(replaceSuffix(fileName,"bmp"));
     QString svgPath=replaceSuffix(fileName,"svg");
     //convert the image to svg by traceToSvg();
     if(tracer->traceToSvg(&image,svgPath)){
         QFile fsvg(svgPath);
         if(fsvg.exists()){
             //use process to invoke gogcode.exe
             QProcess p(0);
             QStringList arg;
             qDebug()<<"start export";
             QString currentPath = QCoreApplication::applicationDirPath();
             qDebug()<<currentPath;
             QString gogcode = currentPath+"/gogcode.exe";
//             QString gogcode= "F:/start/QT/drawApp002/tool/gogcode.exe";
             qDebug()<<replaceSuffix(fileName,"svg");
             arg<< "--file"<<svgPath<<"--output"<<replaceSuffix(fileName,"gcode")<<"--scale"<<"0.5"<<"--dx"<<"-160"<<"--dy"<<"0";
             qDebug()<<arg;
             //start gogcode.exe
             p.start(gogcode,arg);
             p.waitForStarted();
             p.waitForFinished();
             qDebug()<<"export end";
//             fsvg.remove();
             return true;
         }else{
             qDebug()<<"fsvg dont not exist";
             return false;
         }

     }
     return false;
}
bool GraphicsView::exprotSvg(const QString &fileName)
{
     QImage  image = outPutImage();
     QSvgGenerator generator;
     generator.setFileName(fileName);
     generator.setSize(QSize(image.width(), image.height()));
     generator.setViewBox(QRect(0, 0,image.width(), image.height()));
     generator.setTitle(tr("SVG Generator Example Drawing"));
     generator.setDescription(tr("An SVG drawing created by the SVG Generator "
                                  "Example provided with Qt."));
     QPainter painter;
     painter.begin(&generator);
     QRectF igRectF(0,0,image.width(),image.height());
     painter.drawImage(igRectF,image,igRectF);
     painter.end();
     return true;
}
 bool GraphicsView::isSceneEmpty()
 {
     if(!scene())
         return false;
     QList<QGraphicsItem*> itemList = scene()->items();
     bool isEmpty = true;
     for(auto item:itemList){
         if(item->zValue()>0&&item->type()!=QGraphicsItemGroup::Type){
             isEmpty = false;
         }
     }
     return isEmpty;
 }
