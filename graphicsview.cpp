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
#include <QInputDialog>
#include "util.h"
#include "svgtool.h"
#include "tracer.h"
#include "svgitem.h"
#include "filter.h"
#include "toGcode/togcode.h"
#include "toGcode/dpoint.h"
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
    scene()->setSceneRect(0,0,640,440);

    int padding = 20;
    QRect paddingRect(padding,padding,scene()->width()-2*padding,scene()->height()-2*padding);
    // working space
    qreal robot_w = 640;
    qreal robot_h = 440;
    QPointF robot_orgin(paddingRect.center().x(),paddingRect.bottom()-220);
    m_working_rect =QRect(0,0,320*2,60*2);


    m_workingItem = new QGraphicsRectItem(m_working_rect);
    m_workingItem->setPen(QPen(Qt::cyan,1,Qt::DashLine));
    m_workingItem->setVisible(true);

    QGraphicsRectItem *paddingItem = new QGraphicsRectItem(paddingRect);
    paddingItem->setPen(QPen(Qt::black,1,Qt::DashLine));
    paddingItem->setZValue(m_background_zValue);
    //coordinate x and y line
    m_xLineItem = new QGraphicsLineItem(sceneRect().center().x(),0,sceneRect().center().x(),sceneRect().bottom());
    m_xLineItem->setPen(QPen(Qt::green,2));
    m_xLineItem->setZValue(m_background_zValue);


    QImage new_image(":/image/resource/coordinateRect.png");
    QGraphicsPixmapItem * backgroundItem=new QGraphicsPixmapItem();
    QPixmap tempPixmap = QPixmap::fromImage(new_image);
    backgroundItem->setPixmap(tempPixmap);
    backgroundItem->setZValue(-1);

//    scene()->addItem(n_pixmap);

//    scene()->addItem(m_backgroundItem);
//    scene()->addItem(paddingItem);
     scene()->addItem(backgroundItem);
//    scene()->addItem(m_xLineItem);
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
    }else if(suffix=="jpg"||suffix=="png"||suffix=="bmp"){
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

    QImage image(scene()->width(),scene()->height(),QImage::Format_Grayscale8);

    image.fill(Qt::white);//if not use it ,will create black picture in release

    QPainter painter;

    painter.begin(&image);

    m_scene->clearSelection();

    m_scene->render(&painter);

    painter.end();

    return image;

}
void GraphicsView::setGenerate()
{
    QStringList items; //
    items <<"Nomal path"<<"Laser Points"<<"Laser Scan";
    QString dlgTitle="set Generater";
    QString txtLabel="choose Generater";
    int     curIndex=0;
    bool    editable=true;
    bool    ok=false;
    QString text = QInputDialog::getItem(this, dlgTitle,txtLabel,items,curIndex,editable,&ok);
    if (ok && !text.isEmpty())
    {
        if(text==items[0]){
            m_generateType = 0;
        }
        if(text==items[1]){
            m_generateType = 1;
        }
        if(text==items[2]){
            m_generateType = 2;
        }
    }
}
void GraphicsView::setFilter()
{

   QStringList items; //ComboBox 列表的内容
   items <<"Edge Dection"<<"Make Bitmap"<<"Brightnesss";
   QString dlgTitle="set filter";
   QString txtLabel="choose filter";
   int     curIndex=0;
   bool    editable=true;
   bool    ok=false;
   QString text = QInputDialog::getItem(this, dlgTitle,txtLabel,items,curIndex,editable,&ok);
   if (ok && !text.isEmpty())
   {
       if(text==items[0]){
           m_filterType = 0;
       }
       if(text==items[1]){
           m_filterType = 1;
       }
       if(text==items[2]){
           m_filterType = 2;
       }
   }

}
void GraphicsView::test2()
{
    QGraphicsSvgItem *n_svgItem = new QGraphicsSvgItem("F:/start/QT/selectionGroup/ruler/workspace.svg");
    QTransform trans;
//    trans.translate(320,320);

   trans.rotate(-90);
   trans.scale(1,-1);
   trans.translate(-440,-320);
//    trans.translate(-320,-30);
    n_svgItem->setTransform(trans);
    scene()->addItem(n_svgItem);

}
void GraphicsView::test()
{
    qDebug()<<"test start";
    //条目选择输入
//209.179 79.403   209.552 79.403   209.925 79.0299   209.925 79.0299
    DPoint p1(209.179, 79.403);
    DPoint p2(209.552, 79.403);
    DPoint p3(209.925 ,79.0299);
    DPoint p4(209.925, 79.0299);

    CubicBezier c(p1,p2,p3,p4);
    CubicBezier c2(DPoint(100,300),DPoint(200,400),DPoint(300,500),DPoint(400,300));
    BezierToBiarc btb;
    BiarcVector biarcs= btb.ApproxCubicBezier(c,0.5,0.1);
    qDebug()<<"aporoxCubicBezer";
    std::ofstream biarcFile;
    biarcFile.open("C:/Users/Thinkpad/Pictures/gcodevs/biarcs.txt");
    for(BiarcVector::const_iterator b_it = biarcs.begin(); b_it != biarcs.end(); b_it++) {
       biarcFile<<"A1"<<endl;
       biarcFile<<(*b_it).A1.P1.m_x<<" " <<(*b_it).A1.P1.m_y<<endl;
       biarcFile<<(*b_it).A1.P2.m_x<<" " <<(*b_it).A1.P2.m_y<<endl;
       biarcFile<<(*b_it).A1.C.m_x<<" " <<(*b_it).A1.C.m_y<<endl;
       biarcFile<<"A2"<<endl;
       biarcFile<<(*b_it).A2.P1.m_x<<" " <<(*b_it).A2.P1.m_y<<endl;
       biarcFile<<(*b_it).A2.P2.m_x<<" " <<(*b_it).A2.P2.m_y<<endl;
       biarcFile<<(*b_it).A2.C.m_x<<" " <<(*b_it).A2.C.m_y<<endl;
    }
    biarcFile.close();
    qDebug()<<"test end";
}

bool GraphicsView::importImage(const QString &fileName)
{

    Tracer * tracer = new Tracer();
    tracer->setFilter(m_filterType);
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
        if(fileName.isEmpty())
           return  false;
        SvgItem *new_svg = new SvgItem(fileName);

        if(!new_svg)
            return false;
        if(!scene()){
            return false;
        }

        // get a suitable size of new svgitem
        QSizeF size(m_max_import_width,m_max_import_height);

        QSizeF new_size = suitableSize(QSizeF(new_svg->boundingRect().width(),new_svg->boundingRect().height()),size);

        qreal factor = new_size.width()/new_svg->boundingRect().width();

        QTransform transform ;

        transform.scale(factor,factor);
        //set this to make svgtool be used
        new_svg->setFlag(QGraphicsItem::ItemIsSelectable);

        new_svg->setTransform(transform);

        scene()->addItem(new_svg);

        qDebug()<<"import miku boundingRect"<<new_svg->boundingRect();

        return true;
}

bool GraphicsView::exportGcode(const QString &fileName)
{
     if(fileName.isEmpty()||!scene())
         return false;

     if(m_pTool){
         SvgTool *svgTool = static_cast<SvgTool*>(m_pTool);
         svgTool->emptySelection();
     }

     QList<QGraphicsItem*> itemList = scene()->items();
     for(auto item : itemList){
         if(item->isVisible()){
             if(item->type()==SvgItem::Type){
                 SvgItem * tempSvgItem = qgraphicsitem_cast<SvgItem*>(item);
//                 tempSvgItem->setWorkSpace()
                 if(!tempSvgItem->exportGcode(fileName))
                 {
                     return  false;
                 }
             }
         }
     }
     return true;
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
