#include "imageprocess.h"
#include <QBuffer>
#include <QSvgGenerator>
#include <QPainter>
#include <QFile>
#include <QProcess>
#include <QDebug>
//make bitmap
/*check the f is not out of range*/
bool rangeCheck(double f){

    if(f>255){
       return false;
    }else if(f<0){
       return false;
    }else {
       return true;
    }

}

/*generate QRgb*/
QRgb grayToRgb(double f){
    int g= f;
    return QColor(g,g,g).rgb();
}
/* apply lowpass filter (an approximate Gaussian blur) to greymap.
   Lambda is the standard deviation of the kernel of the filter (i.e.,
   the approximate filter radius). */
 void process_lowpass(QImage *pIg, double lambda) {
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
//  qDebug()<<"d"<<d<<"c"<<c<<"B"<<B;
  for (y=0; y<pIg->height(); y++) {
    /* apply low-pass filter to row y */
    /* left-to-right */
    f = g = 0;
    for (x=0; x<pIg->width(); x++) {
      f = f*c + qGray(pIg->pixel(x,y))*d;
      g = g*c + f*d;
      if(!rangeCheck(g))
          continue;
      pIg->setPixel(x,y,grayToRgb(g));
    }

    /* right-to-left */
    for (x=pIg->width()-1; x>=0; x--) {
      f = f*c +  qGray(pIg->pixel(x,y))*d;
      g = g*c + f*d;
      if(!rangeCheck(g))
          continue;
      pIg->setPixel(x,y,grayToRgb(g));
    }

    /* left-to-right mop-up */
    for (x=0; x<pIg->width(); x++) {
      f = f*c;
      g = g*c + f*d;
      if (f+g < 1/255.0) {
         break;
      }
      double d=qGray( pIg->pixel(x,y))+g;
      if(!rangeCheck(d))
          continue;
      pIg->setPixel(x,y,grayToRgb(d));
    }
  }
  for (x=0; x<pIg->width(); x++) {
     /* apply low-pass filter to column x */
     /* bottom-to-top */
     f = g = 0;
     for (y=0; y<pIg->height(); y++) {
       f = f*c + qGray(pIg->pixel(x,y))*d;
       g = g*c + f*d;
       if(!rangeCheck(g))
           continue;
       pIg->setPixel(x,y,grayToRgb(g));
     }

     /* top-to-bottom */
     for (y=pIg->height()-1; y>=0; y--) {
       f = f*c + qGray(pIg->pixel(x,y))*d;
       g = g*c + f*d;
       if(!rangeCheck(g))
           continue;
       pIg->setPixel(x,y,grayToRgb(g));
     }

     /* bottom-to-top mop-up */
     for (y=0; y<pIg->height(); y++) {
       f = f*c;
       g = g*c + f*d;
       if (f+g < 1/255.0) {
           break;
       }
       double d = qGray(pIg->pixel(x,y))+g;
       if(!rangeCheck(d))
           continue;
       pIg->setPixel(x,y,grayToRgb(d));
     }
   }
 }

/* apply highpass filter to greymap. Return 0 on success, 1 on error
   with errno set. */
int process_highpass(QImage *pIg, double lambda) {
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
  process_lowpass(&ig1, lambda);

  /* subtract copy from original */
  int g;
  for (y=0; y<pIg->height(); y++) {
    for (x=0; x<pIg->width(); x++) {
      f = qGray(pIg->pixel(x,y));
      f -= qGray(ig1.pixel(x,y));
      f += 128;    /* normalize! */
      if(!rangeCheck(f))
          continue;
      pIg->setPixel(x,y,grayToRgb(f));
    }
  }
  return 1;
}

int process_threshold(QImage *pIg,double c)
{
    int w=0, h=0;
    double c1;
    int x, y;
    double p;
    w=pIg->width();
    h=pIg->height();
    if(w==0,h==0){
        return false;
    }
     /* thresholding */
    c1 = c * 255;
    QRgb rgbVal = 0;
    int grayVal = 0;
    QColor new_color;

    for (y=0; y<h; y++) {
        for (x=0; x<w; x++) {
          p = pIg->pixel(x,y);
          rgbVal = pIg->pixel(x, y);
          grayVal = qGray(rgbVal);
          if(grayVal>=c1){
              QColor white(Qt::white);
              new_color = white;
          }else{
              QColor black(Qt::black);
              new_color = black;
          }
          pIg->setPixel(x,y,new_color.rgb());
        }
     }
    return true;
}
/*Resize the svg file to szie return a QSvgRenderer*/
QSvgRenderer * process_svgResizeTo(const QString &fileName,QSize size)
{
    //a QBuffer to store the svg data
    QByteArray array;
    QBuffer *buffer =new QBuffer(&array);

    //QSvgRenderer to get the width and height of svg
    QSvgRenderer *renderer = new QSvgRenderer(fileName);

    if(!renderer)
        return 0;

    qreal new_width=size.width();
    qreal new_height=size.height();

    //is width longer than height
    bool width_than_height=renderer->defaultSize().width()>renderer->defaultSize().height()? true:false;

    qreal max_length=renderer->defaultSize().width()>renderer->defaultSize().height()? renderer->defaultSize().width():renderer->defaultSize().height();

    //calculate the appropriate width and height
    if(max_length>size.width()){
        if(width_than_height){
             new_width=size.width();
             qreal factor = new_width/renderer->defaultSize().width();
             new_height=factor*renderer->defaultSize().height();
        }else{
            new_height=size.height();
            qreal factor = new_height/renderer->defaultSize().height();
            new_width=factor*renderer->defaultSize().width();
        }
    }
    //a QSvgGenerator to create a new svg
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
    //use painter to resize the svg to a new size ,store new svg in buffer
    painter.begin(&generator);
    renderer->render(&painter);
    painter.end();

    buffer->open(QIODevice::ReadOnly);
    QSvgRenderer *newRenderer =new QSvgRenderer(buffer->readAll());
    buffer->close();
    if(!newRenderer)
        return 0;
    return newRenderer;

}
QSvgRenderer * process_imageToSvg(const QString &fileName)
{
    QSvgRenderer *pSvgRenderer;
    QImage *pImage = new QImage(fileName);
    if(!pImage){
        qDebug()<<"pImage =0";
        return 0;
    }
    if(!process_highpass(pImage,4)){
        return 0;
    }
    if(!process_threshold(pImage,0.45)){
        return 0;
    }
    QString bmpFile = replaceSuffix(fileName,"bmp");
    QFile* imageFile= new QFile(bmpFile);
    pImage->save(bmpFile);
    imageFile->waitForReadyRead(1000);
    qDebug()<<"make bitmap success";
    if(imageFile->exists()){
        QProcess p(0);
        QStringList arg1;
        // potrace --svg --flat [filename
        //F:/start/QT/build-drawApp-Desktop_Qt_5_13_0_MSVC2015_64bit-Debug/debug
        QString potrace= "F:/start/QT/drawApp/tool/potrace.exe";
        arg1<< "--svg" << "--flat" <<bmpFile;
        p.start(potrace,arg1);
        p.waitForStarted();
        p.waitForFinished();
         qDebug()<<"potrace success";
    }else{
       return 0;
    }
    QString svgFile = replaceSuffix(bmpFile,"svg");
    pSvgRenderer = new QSvgRenderer(svgFile);
    if(!pSvgRenderer){
        return 0;
    }
    return pSvgRenderer;


}

//faster than Qt image Process
QString makeBitmap(const QString &fileName)
{
    QProcess p(0);
    QStringList arg1;
    // potrace --svg --flat [filename
    //F:/start/QT/build-drawApp-Desktop_Qt_5_13_0_MSVC2015_64bit-Debug/debug
    QString mkbitmap= "F:/start/QT/drawApp/tool/mkbitmap.exe";
    arg1<< "--s" << " 1 " <<fileName;
    p.start(mkbitmap,arg1);
    p.waitForStarted();
    p.waitForFinished();
    return replaceSuffix(fileName,"pbm");
}
QString potrace(const QString &fileName)
{
    QProcess p(0);
    QStringList arg1;
    // potrace --svg --flat [filename
    //F:/start/QT/build-drawApp-Desktop_Qt_5_13_0_MSVC2015_64bit-Debug/debug
    QString potrace= "F:/start/QT/drawApp/tool/potrace.exe";
    arg1<< "--svg" << "--flat" <<fileName;
    p.start(potrace,arg1);
    p.waitForStarted();
    p.waitForFinished();
    return replaceSuffix(fileName,"svg");
}
QString replaceSuffix(QString str,QString suf)
{
    int no_suffixes=str.lastIndexOf('.');
    QString replaced_Str =str.replace(no_suffixes,str.length()-1,"."+suf);
    return replaced_Str;
}
QString getSuffix(QString str)
{
    int no_suffixes=str.lastIndexOf('.');
    QStringList l=str.split(".");
    return l.at(l.length()-1);

}
