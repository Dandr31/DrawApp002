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

#include "svgtool.h"
/*
 The m31 (dx) and m32 (dy) elements specify horizontal and vertical translation.
 The m11 and m22 elements specify horizontal and vertical scaling. The m21 and m12
 elements specify horizontal and vertical shearing. And finally, the m13 and m23
 elements specify horizontal and vertical projection, with m33 as an additional
 projection factor.
*/
QTransform GraphicsView::transform_set(int l,int n,QTransform o_transform,qreal value){
    qreal m[4][4];
    m[1][1] = o_transform.m11();
    m[1][2] = o_transform.m12();
    m[1][3]=  o_transform.m13();
    m[2][1] = o_transform.m21();
    m[2][2] = o_transform.m22();
    m[2][3] = o_transform.m23();
    m[3][1] = o_transform.m31();
    m[3][2]=  o_transform.m32();
    m[3][3] = o_transform.m33();
    if(l>0&&l<4&&n>0&&n<4){
        m[l][n]=value;
    }
    o_transform.setMatrix(m[1][1],m[1][2],m[1][3],m[2][1],m[2][2],m[2][3],m[3][1],m[3][2],m[3][3]);
    return o_transform  ;
}
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

 // Prepare background check-board pattern
 QPixmap tilePixmap(64, 64);
 tilePixmap.fill(Qt::white);
 QPainter tilePainter(&tilePixmap);
 QColor color(220, 220, 220);
 tilePainter.fillRect(0, 0, 32, 32, color);
 tilePainter.fillRect(32, 32, 32, 32, color);
 tilePainter.end();
 setBackgroundBrush(tilePixmap);
 initDrawTool();
 openFile("F://start//QT//drawApp002//res//miku.svg");
}
void GraphicsView::initDrawTool()
{
    scene()->clear();
    resetTransform();
    scene()->setSceneRect(0,0,600,400);
    m_outlineItem = new QGraphicsRectItem(120,120,100,50);
    m_outlineItem->setPen(QPen(Qt::red,2));
	m_outlineItem->setBrush(Qt::NoBrush);
	m_outlineItem->setVisible(true);
    m_outlineItem->setZValue(1);

    m_outlineItem->setFlag(QGraphicsItem::ItemIsSelectable);
    m_backgroundItem = new QGraphicsRectItem(120,130,50,50);
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

    scene()->addItem(n_pixmap);

    scene()->addItem(m_backgroundItem);
    scene()->addItem(m_outlineItem);
    m_pTool = new SvgTool(scene());

}
QSize GraphicsView::svgSize() const
{
    return m_svgItem ? m_svgItem->boundingRect().size().toSize() : QSize();
}
bool GraphicsView::openFile(const QString &fileName)
{
    QGraphicsScene *s = scene();

    const bool drawBackground = (m_backgroundItem ? m_backgroundItem->isVisible() : false);
    //const bool drawOutline = (m_outlineItem ? m_outlineItem->isVisible() : true);

    QScopedPointer<QGraphicsSvgItem> svgItem(new QGraphicsSvgItem(fileName));
    if (!svgItem->renderer()->isValid())
        return false;

//    s->clear();
//    resetTransform();

    QGraphicsTextItem *textItem = new QGraphicsTextItem(QString("Dandr 31"));
//    textItem->
    textItem->setFlag(QGraphicsItem::ItemIsSelectable);
    QFont font("Helvetica [Cronyx]", 30,QFont::Bold);
    textItem->setFont(font);
    textItem->setTextWidth(200);


    m_svgItem = svgItem.take();
    m_svgItem->setFlags(QGraphicsItem::ItemClipsToShape);
    m_svgItem->setCacheMode(QGraphicsItem::NoCache);
    m_svgItem->setZValue(0);
    m_svgItem->setFlag(QGraphicsItem::ItemIsSelectable);
//    s->addItem(m_svgItem);
//    s->addItem(m_outlineItem);
//    s->addItem(textItem);
//    s->setSceneRect(m_outlineItem->boundingRect().adjusted(-10, -10, 10, 10));
    return true;
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
    QImage image(m_scene->width(),m_scene->height() ,QImage::Format_ARGB32);
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
//   importSvg("F://start//QT//drawApp002//res//mikubpIg->svg");
//   importSvg("F://start//QT//drawApp002//res//miku212.svg");
//   QImage image = outPutImage();
     QImage image("F://start//QT//res//miku.jpg");
//   qDebug()<<"QImage"<<image;
//   qDebug()<<"QRgb"<<image.pixel(100,100)<<image.pixel(400,100);
//   qDebug()<<qGray(image.pixel(100,100));
//   if(m_outlineItem)
//   {
//       QColor c(Qt::cyan);
//       qDebug()<<qGray(c.rgb())<<c.rgb()<<c;
//       QColor g(qGray(c.rgb()),qGray(c.rgb()),qGray(c.rgb()));
//       m_outlineItem->setPen(QPen(g,3));
//   }

   qreal cutoff=0.45;
   int y ,x;
   int h = image.height();
   int w = image.width();
   highpass(&image,4);
//   lowpass(&image,4);
   QImage new_image = threshold(image,cutoff);
//   new_image.save("F://start//QT//res//mikuQT.jpg");//work well
//   QImage new_image=image.copy(0,0,w,h);
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
   show_scene->addItem(n_pixmap);

}
bool GraphicsView::importSvg(const QString &fileName)
{
        QByteArray array;
        QBuffer *buffer =new QBuffer(&array);
        QSvgRenderer *renderer = new QSvgRenderer(fileName);
        if(!renderer)
            return false;
        qreal new_width=m_max_import_width;
        qreal new_height=m_max_import_height;
        bool width_than_height=renderer->defaultSize().width()>renderer->defaultSize().height()? true:false;
        qreal max_length=renderer->defaultSize().width()>renderer->defaultSize().height()? renderer->defaultSize().width():renderer->defaultSize().height();
        if(max_length>m_max_import_width){
            if(width_than_height){
                 new_width=m_max_import_width;
                 qreal factor = new_width/renderer->defaultSize().width();
                 new_height=factor*renderer->defaultSize().height();
            }else{
                new_height=m_max_import_height;
                qreal factor = new_height/renderer->defaultSize().height();
                new_width=factor*renderer->defaultSize().width();
            }
        }
        QSvgGenerator generator;
        if(!buffer){
            return false;
        }
        generator.setOutputDevice(buffer);
    //    generator.setFileName("F://start//QT//drawApp002//res//buffer.svg");
        generator.setSize(QSize(new_width, new_height));
        generator.setViewBox(QRect(0, 0, new_width, new_height));

        QImage image;
        QPainter painter;
        painter.begin(&generator);
        renderer->render(&painter);
        painter.end();
        buffer->open(QIODevice::ReadOnly);
        QSvgRenderer *newRenderer =new QSvgRenderer(buffer->readAll());
        buffer->close();
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
}
QRectF GraphicsView::selectItemsBoundingRect()
{
    if(!scene()){
        return QRectF();
    }
    QList<QGraphicsItem *> selectedItems = scene()->selectedItems();
    //todo add a selected limit
    if ( selectedItems.count() >0 ){
        QGraphicsItemGroup* pGroup=scene()->createItemGroup(scene()->selectedItems());
        QRectF boundingRect = pGroup->boundingRect();
        scene()->destroyItemGroup(pGroup);
        return boundingRect;
    }

}
void GraphicsView::selectItemAtBox(QRectF rect)
{
    if(!scene()){
        return;
    }
    QList<QGraphicsItem*> items = scene()->items();
    if(items.count()>0){
        for (QGraphicsItem* item : items)
        {
            if(!item->isVisible())
                continue;
            if(item->type()!=QGraphicsRectItem::Type)
                continue;
            if (rect.intersects(item->boundingRect()) && item->boundingRect().intersects(rect))
                item->setSelected(true);
        }
    }

}
QImage  GraphicsView::threshold(QImage image,double c)
{

    int w=0, h=0;
    double c1;
    int x, y;
    double p;
    w=image.width();
    h=image.height();
    QImage new_image(w,h,QImage::Format_Grayscale8);
     /* thresholding */
    c1 = c * 255;
    QRgb rgbVal = 0;
    int grayVal = 0;
    QColor new_color;

    for (y=0; y<h; y++) {
        for (x=0; x<w; x++) {
          p = image.pixel(x,y);
          rgbVal = image.pixel(x, y);
          grayVal = qGray(rgbVal);

          if(grayVal>=c1){
              QColor white(Qt::white);
              new_color = white;
          }else{
              QColor black(Qt::black);
              new_color = black;
          }
          new_image.setPixel(x,y,new_color.rgb());
//          new_image->setPixel(x,y,p<c1);
//          new_image.setPixel(x,y,QColor(grayVal,grayVal,grayVal).rgb());
        }
      }


    return new_image;
}
/* apply lowpass filter (an approximate Gaussian blur) to greymap.
   Lambda is the standard deviation of the kernel of the filter (i.e.,
   the approximate filter radius). */
static void lowpass(QImage *pIg, double lambda) {
  double f, g;
  double c, d;
  double B;
  int x, y;

  if (pIg->height() == 0 || pIg->width() == 0) {
    return;
  }
  //if lambda = 4 that d 0.390388 c 0.609612 B 1.125
  /* calculate filter coefficients from given lambda */
  B = 1+2/(lambda*lambda);
  c = B-sqrt(B*B-1);
  d = 1-c;
  qDebug()<<"d"<<d<<"c"<<c<<"B"<<B;
  for (y=0; y<pIg->height(); y++) {
    /* apply low-pass filter to row y */
    /* left-to-right */
    f = g = 0;
    for (x=0; x<pIg->width(); x++) {
      f = f*c + qGray(pIg->pixel(x,y))*d;
      g = g*c + f*d;
      pIg->setPixel(x,y,QColor(g,g,g).rgb());
    }

    /* right-to-left */
    for (x=pIg->width()-1; x>=0; x--) {
      f = f*c +  qGray(pIg->pixel(x,y))*d;
      g = g*c + f*d;
      pIg->setPixel(x,y,QColor(g,g,g).rgb());
    }

    /* left-to-right mop-up */
    for (x=0; x<pIg->width(); x++) {
      f = f*c;
      g = g*c + f*d;
      if (f+g < 1/255.0) {
    break;
      }
      double d=qGray( pIg->pixel(x,y))+g;
      pIg->setPixel(x,y,QColor(d,d,d).rgb());
    }
  }
  for (x=0; x<pIg->width(); x++) {
     /* apply low-pass filter to column x */
     /* bottom-to-top */
     f = g = 0;
     for (y=0; y<pIg->height(); y++) {
       f = f*c + qGray(pIg->pixel(x,y))*d;
       g = g*c + f*d;
       pIg->setPixel(x,y,QColor(g,g,g).rgb());
     }

     /* top-to-bottom */
     for (y=pIg->height()-1; y>=0; y--) {
       f = f*c + qGray(pIg->pixel(x,y))*d;
       g = g*c + f*d;
       pIg->setPixel(x,y,QColor(g,g,g).rgb());
     }

     /* bottom-to-top mop-up */
     for (y=0; y<pIg->height(); y++) {
       f = f*c;
       g = g*c + f*d;
       if (f+g < 1/255.0) {
           break;
       }
       double d = qGray(pIg->pixel(x,y))+g;
       pIg->setPixel(x,y,QColor(d,d,d).rgb());
     }
   }
 }

/* apply highpass filter to greymap. Return 0 on success, 1 on error
   with errno set. */
static int highpass(QImage *pIg, double lambda) {
  double f;
  int x, y;
  if(!pIg){
      return 0;
  }
  if (pIg->width() == 0 ||pIg->height() == 0) {
    return 0;
  }

  /* create a copy */
  QImage ig1;
  ig1 = pIg->copy(0,0,pIg->width(),pIg->height());

  /* apply lowpass filter to the copy */
  lowpass(&ig1, lambda);

  /* subtract copy from original */
  for (y=0; y<pIg->height(); y++) {
    for (x=0; x<pIg->width(); x++) {
      f = qGray(pIg->pixel(x,y));
      f -= qGray(ig1.pixel(x,y));
      f += 128;    /* normalize! */
      pIg->setPixel(x,y,QColor(f,f,f).rgb());
    }
  }
  return 0;
}


