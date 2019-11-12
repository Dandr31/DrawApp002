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
#include "imageprocess.h"
#include "svgtool.h"
GraphicsView::GraphicsView(QWidget *parent)
: QGraphicsView(parent)
 , m_svgItem(nullptr)
 , m_backgroundItem(nullptr)
 , m_outlineItem(nullptr)

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
    scene()->setSceneRect(0,0,600,400);
    m_outlineItem = new QGraphicsRectItem(100,100,100,50);
    m_outlineItem->setPen(QPen(Qt::red,2));
	m_outlineItem->setBrush(Qt::NoBrush);
	m_outlineItem->setVisible(true);
    m_outlineItem->setZValue(1);

    m_outlineItem->setFlag(QGraphicsItem::ItemIsSelectable);
    m_backgroundItem = new QGraphicsRectItem(200,200,50,50);
    QTransform transform=m_backgroundItem->transform();
    QPointF center= m_backgroundItem->boundingRect().center();
    transform.translate(center.x(),center.y());
    transform.rotate(45);
    transform.scale(2,3);
    transform.translate(-center.x(),-center.y());
    QGraphicsRectItem *child=new QGraphicsRectItem(0,0,3,3);
    child->setPen(QPen(Qt::red,2));
    child->setPos(m_outlineItem->boundingRect().center());
    child->setParentItem(m_outlineItem);
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
    m_backgroundItem->setTransform(transform);
    m_backgroundItem->setPen(QPen(Qt::black,1));;
    m_backgroundItem->setZValue(1);
    m_backgroundItem->setFlag(QGraphicsItem::ItemIsSelectable);
    m_backgroundItem->setPos(100,100);

//    scene()->addItem(n_pixmap);

//    scene()->addItem(m_backgroundItem);
//    scene()->addItem(m_outlineItem);
    m_pTool = new SvgTool(scene());

}
QSize GraphicsView::svgSize() const
{
    return m_svgItem ? m_svgItem->boundingRect().size().toSize() : QSize();
}
bool GraphicsView::openFile(const QString &fileName)
{
    QGraphicsScene *s = scene();
    QString suffix =getSuffix(fileName);
    if(suffix=="svg"){
       return importSvg(fileName);
    }else if(suffix=="jpg"||suffix=="png"){
       QImage image(fileName);
       if(image.width()==0){
           return false;
       }
       image.convertTo(QImage::Format_Grayscale8);
       QString bmpPath = replaceSuffix(fileName,"bmp");
       image.save(bmpPath);

       return importImage(bmpPath);
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
void GraphicsView::drawBackground(QPainter *p, const QRectF &)
{
     Q_UNUSED(p)
    //draw background in there
//    p->save();
//    p->resetTransform();
//    p->drawTiledPixmap(viewport()->rect(), backgroundBrush().texture());
//    p->restore();
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

    if(m_pTool){
        SvgTool *svgTool = static_cast<SvgTool*>(m_pTool);
        svgTool->emptySelectionGroup();
    }

    QImage image(m_scene->width(),m_scene->height() ,QImage::Format_Grayscale8);

    QPainter painter;

    painter.begin(&image);

    m_scene->clearSelection();

    m_scene->render(&painter);

    painter.end();
//    image.save(path);
    return image;

}

void GraphicsView::test()
{
     exprotSvg("F://start//QT//res//output1112.svg");
     return;
//   importSvg("F://start//QT//drawApp002//res//mikubpIg->svg");
//   importSvg("F://start//QT//drawApp002//res//miku212.svg");
//   QImage image = outPutImage();
     QImage image("F://start//QT//res//miku.jpg");
     makeBitmapByPotrace("C:/Users/Thinkpad/Pictures/timg.bmp");
     return;
     //importSvg test
//     importSvg("F://start//QT//res//1111miku2.svg");
//     return;


//   qDebug()<<"QImage"<<image;
//   qDebug()<<"QRgb"<<image.pixel(100,100)<<image.pixel(400,100);
//   qDebug()<<qGray(image.pixel(100,100));
//   if(m_outlineItem)
//   {
//       QColor c(288,288,288);
//       qDebug()<<qGray(c.rgb())<<c.rgb()<<c;
//       QColor g(qGray(c.rgb()),qGray(c.rgb()),qGray(c.rgb()));
//       m_outlineItem->setPen(QPen(g,3));
//   }

   qreal cutoff=0.45;
   int y ,x;
   int h = image.height();
   int w = image.width();
//   process_highpass(&image,4);

//   lowpass(&image,4);
//   QImage new_image = threshold(image,cutoff);
//   process_threshold(&image,cutoff);

   QImage new_image=image.copy();
//   new_image.save("F://start//QT//res//mikuQT.jpg");//work well
//   QImage new_image=image.copy(0,0,w,h);
   //process_imageToSvg
   QString bmpFile = makeBitmapByPotrace("F://start//QT//res//mikuQT.jpg");

//   QString bmpFile = makeBitmap("F://start//QT//res//mikuQT.jpg");
   QString svgFile = potrace(bmpFile);

   qDebug()<<"svgFile"<<svgFile;
   QSvgRenderer *pRenderer = process_svgResizeTo(svgFile,QSize(400,300));
   qDebug()<<"pRenderer"<<pRenderer->defaultSize();
   QGraphicsSvgItem *pSvg = new QGraphicsSvgItem();
   if(!pRenderer){
       qDebug()<<"pRenderer =0";
       return ;
   }
   pSvg->setSharedRenderer(pRenderer);
   //convert to grayscale than process it
//   QImage g_image("F://start//QT//res//mikuQT.jpg");
//   g_image= g_image.convertToFormat(QImage::Format_Grayscale8);
//   g_image.save("F://start//QT//res//mikuQT111209.bmp");
   //show
   QGraphicsView *show_view = new QGraphicsView();
   QGraphicsScene *show_scene = new QGraphicsScene();
   if(!show_view||!show_scene){
       return;
   }
   show_scene->clear();
   show_scene->setSceneRect(0,0,600,400);
   show_view->setScene(show_scene);
   show_view->setDragMode(ScrollHandDrag);
   show_view->show();

   QGraphicsPixmapItem * n_pixmap=new QGraphicsPixmapItem();
   QPixmap tempPixmap = QPixmap::fromImage(new_image);
   n_pixmap->setPixmap(tempPixmap);
   n_pixmap->setPos(0,0);
//   show_scene->addItem(n_pixmap);
   show_scene->addItem(pSvg);
}

bool GraphicsView::importImage(const QString &fileName)
{

    QString bitmapPath = makeBitmapByPotrace(fileName);
    qDebug()<<"bitmapPath"<<bitmapPath;
    if(bitmapPath==0){
        return false;
    }
    QFile bitmapFile(bitmapPath);
    if(!bitmapFile.exists()){
        return false;
    }
    qDebug()<<"bitmapFile"<<bitmapFile.fileName();
    QString svgPath = potrace(bitmapPath);

    qDebug()<<"svgFile"<<svgPath;

    QSvgRenderer *pRenderer = process_svgResizeTo(svgPath,QSize(400,300));

    qDebug()<<"pRenderer"<<pRenderer->defaultSize();

    QGraphicsSvgItem *pSvg = new QGraphicsSvgItem();

    if(!pRenderer){
        qDebug()<<"pRenderer =0";
        return false ;
    }
    pSvg->setSharedRenderer(pRenderer);

    pSvg->setFlag(QGraphicsItem::ItemIsSelectable);

    if(!pSvg->scene()){
        scene()->addItem(pSvg);
    }

    QFile bmpFile(fileName);
    if(bmpFile.exists()){
        bmpFile.remove();
    }
    if(bitmapFile.exists()){
        bitmapFile.remove();
    }
    QFile svgFile(svgPath);
    if(svgFile.exists())
        svgFile.remove();
    return true;
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

bool GraphicsView::exportGcode(const QString &fileName){

    if(!scene()){
        return false;
    }

    if(scene()->items().count()==0){
        return false;
    }

    if (fileName.isEmpty())
           return false ;

    QString bmpPath = replaceSuffix(fileName,"bmp");
    //render scene to a image , QImage::Format_Grayscale8
   QImage image = outPutImage();
   QFile bmpFile(bmpPath);
   if(image.save(bmpPath)){
        qDebug()<<bmpPath;
        qDebug()<<"save success";
   }
   if(bmpFile.exists()){
       QProcess p(0);
       QStringList arg1;
       // potrace --svg --flat [filename
       //F:/start/QT/build-drawApp-Desktop_Qt_5_13_0_MSVC2015_64bit-Debug/debug
       QString mkbitmap= "F:/start/QT/drawApp002/tool/mkbitmap.exe";
       arg1<< "--s" << "1" <<bmpPath;
       p.start(mkbitmap,arg1);
       p.waitForStarted();
       p.waitForFinished();

       QStringList arg2;
       // potrace --svg --flat [filename
       //F:/start/QT/build-drawApp-Desktop_Qt_5_13_0_MSVC2015_64bit-Debug/debug
       QString potrace= "F:/start/QT/drawApp002/tool/potrace.exe";
       arg2<< "--svg" << "--flat" <<replaceSuffix(bmpPath,"pbm");
       p.start(potrace,arg2);
       p.waitForStarted();
       p.waitForFinished();

       QStringList arg3;
       // gogcode --file [filename.svg] --output [filename.gcode] --scale [n]
       //F:/start/QT/build-drawApp-Desktop_Qt_5_13_0_MSVC2015_64bit-Debug/debug
       QString gogcode= "F:/start/QT/drawApp002/tool/gogcode.exe";
       arg3<< "--file"<<replaceSuffix(bmpPath,"svg")<<"--output"<<replaceSuffix(bmpPath,"gcode");
       p.start(gogcode,arg3);
       p.waitForStarted();
       p.waitForFinished();

    //  remove the  intermediate documents
//       QFile bmpFile(bmpPath);
//       if(bmpFile.exists()){
//           bmpFile.remove();
//       }
//       QFile svgFile(replaceSuffix(bmpPath,"svg"));
//       if(svgFile.exists())
//            svgFile.remove();

//       QFile pbmFile(replaceSuffix(bmpPath,"pbm"));
//       if(pbmFile.exists())
//           pbmFile.remove();

    }else{
           qDebug()<<"imageFile is not exists";
    }

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
