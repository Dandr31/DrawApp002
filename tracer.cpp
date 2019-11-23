#include "tracer.h"
#include <stdio.h>
#include <stdlib.h>
#include <QFile>
#include <QTextStream>
#include <QtDebug>
#include <potrace/svg.h>
#define WIDTH 200
#define HEIGHT 200
inline QString Tracer::toQString(double t){
    return QString::number(t);
}
Tracer::Tracer()
{

}
bool Tracer::traceToSvg(QImage *pImage,const QString &filename)
{
    potrace_bitmap_t *bm;
    potrace_param_t *param;
    potrace_state_t *st;

    bm = mkbitmap(pImage,0.45);
    if(!bm){
        qDebug()<<"bm==0";
        return 0;
    }
    /* set tracing parameters, starting from defaults */
    param = potrace_param_default();
    if (!param) {
      return 0;
    }
    param->turdsize = 0;

    /* trace the bitmap */
    st = potrace_trace(param, bm);
    if (!st || st->status != POTRACE_STATUS_OK) {
      qDebug()<<"st is empty";
      return 0;
    }
    qDebug()<<"svg save";
    /*save to svg convert  QString to const char * */
    FILE* fout =fopen(filename.toLocal8Bit().data(), "w");
    if(!fout){
        qDebug()<<"fout == 0";
        return 0;
    }
    //int page_svg(FILE *fout, potrace_path_t *plist);
    qDebug()<<"imginfo "<<bm->w<<bm->h;
    imginfo_t imginfo;
    imginfo.width=bm->w;
    imginfo.height=bm->h;
    int r=page_svg(fout,st->plist,&imginfo);

    qDebug()<<"save end";
    fclose(fout);
    bm_free(bm);
    potrace_state_free(st);
    potrace_param_free(param);
    if(r!=0)
    {
        qDebug()<<"save failed";
        return false;
    }else if(r==0){
        qDebug()<<"save success";
        return true;
    }
}
potrace_bitmap_t * Tracer::bm_new(int w, int h) {
   potrace_bitmap_t *bm;
   int dy = (w + BM_WORDBITS - 1) / BM_WORDBITS;

   bm = (potrace_bitmap_t *) malloc(sizeof(potrace_bitmap_t));
   if (!bm) {
    return NULL;
   }
   bm->w = w;
   bm->h = h;
   bm->dy = dy;
   bm->map = (potrace_word *) calloc(h, dy * BM_WORDSIZE);
   if (!bm->map) {
     free(bm);
     return NULL;
   }
   return bm;
}

/* free a bitmap */
void Tracer:: bm_free(potrace_bitmap_t *bm) {
  if (bm != NULL) {
    free(bm->map);
  }
  free(bm);
}
void Tracer::test2()
{
    int x, y, i;
    potrace_bitmap_t *bm;
    potrace_param_t *param;
    potrace_path_t *p;
    potrace_state_t *st;
    int n, *tag;
    potrace_dpoint_t (*c)[3];
    QImage *pImage =new QImage("F:\\start\\QT\\res\\mikuQT.jpg");
    qDebug()<<pImage->width()<<pImage->height();
    bm = mkbitmap(pImage,0.45);
    if(!bm){
        qDebug()<<"bm==0";
        return;
    }

    /* set tracing parameters, starting from defaults */
    param = potrace_param_default();
    if (!param) {
      return ;
    }
    param->turdsize = 0;

    /* trace the bitmap */
    st = potrace_trace(param, bm);
    if (!st || st->status != POTRACE_STATUS_OK) {

      return ;
    }
    bm_free(bm);
    QString data,space=" ";
    /* draw each curve */
    p = st->plist;
    while (p != NULL) {
       n = p->curve.n;
       tag = p->curve.tag;
       c = p->curve.c;
       data=data+QString("%1 %2 m\n").arg( c[n-1][2].x).arg( c[n-1][2].y);
//       "%f %f moveto\n", c[n-1][2].x, c[n-1][2].y
       for (i=0; i<n; i++) {
         switch (tag[i]) {
         case POTRACE_CORNER:
               data+=QString("%1 %2 l\n").arg( c[i][1].x).arg(c[i][1].y);
               data+=QString("%1 %2 l\n").arg(c[i][2].x).arg( c[i][2].y);
//      "%f %f lineto\n", c[i][1].x, c[i][1].y
//       "%f %f lineto\n", c[i][2].x, c[i][2].y
       break;
         case POTRACE_CURVETO:
               data+=QString("%1 %2 %3 %4 %5 %6 c\n")
                       .arg(c[i][0].x).arg(c[i][0].y)
                       .arg(c[i][1].x).arg(c[i][1].y)
                       .arg(c[i][2].x).arg(c[i][2].y);
                     /*"%f %f %f %f %f %f curveto\n",
                      c[i][0].x, c[i][0].y,
                      c[i][1].x, c[i][1].y,
                      c[i][2].x, c[i][2].y*/
       break;
         }
       }
       /* at the end of a group of a positive path and its negative
          children, fill. */
       if (p->next == NULL || p->next->sign == '+') {
//         "0 setgray fill\n"
           data+="0 setgray fill";
       }
       p = p->next;
     }
    qDebug()<<data;
    QFile fout("F:\\start\\QT\\selectionGroup\\res\\testdata.txt");
    if (!fout.open(QIODevice::WriteOnly | QIODevice::Text))
          return;
    QTextStream out(&fout);
    out<<data;
    fout.close();

    potrace_state_free(st);
    potrace_param_free(param);
}
void Tracer::test()
{

    potrace_bitmap_t *bm;
    potrace_param_t *param;

    potrace_state_t *st;

    QString name="jiang";
    QString source="F:\\start\\QT\\res\\jiang.jpg";
    QString target ="F:\\start\\QT\\selectionGroup\\res\\";
    const char * target_svg = "F:\\start\\QT\\selectionGroup\\res\\jiang.svg";
    QImage *pImage =new QImage(source);
    qDebug()<<pImage->width()<<pImage->height();
    bm = mkbitmap(pImage,0.45);
    if(!bm){
        qDebug()<<"bm==0";
        return;
    }
    write_bm2(target+name+QString(".pbm"),bm);
    /* set tracing parameters, starting from defaults */
    param = potrace_param_default();
    if (!param) {
      return ;
    }
    param->turdsize = 0;

    /* trace the bitmap */
    st = potrace_trace(param, bm);
    if (!st || st->status != POTRACE_STATUS_OK) {
      qDebug()<<"st is empty";
      return ;
    }
    qDebug()<<"svg save";
    /*save to svg*/
    FILE* fout =fopen(target_svg, "w");
    if(!fout){
        qDebug()<<"fout == 0";
        return;
    }
    //int page_svg(FILE *fout, potrace_path_t *plist);
    qDebug()<<"imginfo "<<bm->w<<bm->h;
    imginfo_t imginfo;
    imginfo.width=bm->w;
    imginfo.height=bm->h;
    int r=page_svg(fout,st->plist,&imginfo);
    if(r==1)
    {
        qDebug()<<"save failed";
    }else if(r==0){
        qDebug()<<"save success";
    }
    qDebug()<<"save end";
    fclose(fout);
    bm_free(bm);
    potrace_state_free(st);
    potrace_param_free(param);
}
potrace_bitmap_t * Tracer::imageToBitmap(QImage *pImage)
{

    if(!pImage){
//        qDebug()<<"pImage is empty";
        return 0;
    }
    if(pImage->width()==0||pImage->height()==0){
//        qDebug()<<"width == 0||height == 0";
        return 0;
    }
    int x,y;
    int width = pImage->width();
    int height = pImage->height();

    potrace_bitmap_t *bm;

    bm = bm_new(width, height);

    if (!bm) {
//      qDebug()<<"bm is empty";
      return 0 ;
    }
    /* fill the bitmap with some pattern */
    for (y=0; y<height; y++) {
      for (x=0; x<width; x++) {
        //inverted the direction and color of  pImage
        BM_PUT(bm, x, y,255-IG_UGET(pImage,x,height-y-1));
      }
    }
    return bm;
}
bool Tracer::write_bm(const QString &fileName,potrace_bitmap_t *bm)
{
    if(!bm)
        return false;

    QFile fout(fileName);
    if (!fout.open(QIODevice::WriteOnly | QIODevice::Text))
          return false;
    int w, h, bpr, y, i, c;

    w = bm->w;
    h = bm->h;

    bpr = (w+7)/8;

    QByteArray bArray;
    QString head=QString("P4\n%1 %2\n").arg(w).arg(h);
    fout.write(head.toUtf8());
//    bArray+=head.remove(head.length()-1);
    QString data;
    for (y=h-1; y>=0; y--) {
      for (i=0; i<bpr; i++) {
         c = (*bm_index(bm, i*8, y) >> (8*(BM_WORDSIZE-1-(i % BM_WORDSIZE)))) & 0xff;
         fout.putChar(c);
       }
    }
//    fout.write(bArray);
//    fout.write(data.toUtf8());
    fout.close();
}
bool Tracer::write_bm2(const QString &fileName,potrace_bitmap_t *bm)
{
    if(!bm)
        return false;
    std::string tempStr = fileName.toStdString();
    const char * file_n = tempStr.data();
    if (file_n == NULL || strcmp(file_n, "-") == 0) {
       return false;
     }
    FILE * fout =fopen(file_n, "wb");
 /*always add a white line,if use QFile*/
//    QFile f2("F:\\start\\QT\\selectionGroup\\res\\bit2.pbm");
//    if (!f2.open(QIODevice::WriteOnly | QIODevice::Text))
//          return false;
    if(!fout)
        return false;
    int w, h, bpr, y, i, c;

     w = bm->w;
     h = bm->h;

     bpr = (w+7)/8;
//     QString head=QString("P4\n%1 %2\n").arg(w).arg(h);
//     fprintf(fout, head.toUtf8());
     fprintf(fout, "P4\n%d %d\n", w, h);
//     f2.write(head.toUtf8());
     for (y=h-1; y>=0; y--) {
       for (i=0; i<bpr; i++) {
           c = (*bm_index(bm, i*8, y) >> (8*(BM_WORDSIZE-1-(i % BM_WORDSIZE)))) & 0xff;
          fputc(c, fout);
//          f2.putChar(c);
        }
     }
//     f2.close();
     fclose(fout);
     return true;
}
potrace_bitmap_t* Tracer::mkbitmap(QImage *pImage,double c)
{
    if(!pImage)
        return 0;
    if(pImage->width()==0||pImage->height()==0)
        return 0;
    QImage image;

    image = pImage->convertToFormat(QImage::Format_Grayscale8);

    if(!highpass(&image))
            return 0;
    if(!threshold(&image,c))
            return 0;
//    image.save("F:\\start\\QT\\selectionGroup\\res\\test1.bmp");

    return imageToBitmap(&image);

}
/* apply lowpass filter (an approximate Gaussian blur) to greymap.
   Lambda is the standard deviation of the kernel of the filter (i.e.,
   the approximate filter radius). */
bool Tracer::lowpass(QImage *pIg, double lambda) {
  double f, g;
  double c, d;
  double B;
  int x, y;

  if (pIg->height() == 0 || pIg->width() == 0) {
    return false;
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
        f = f*c + IG_UGET(pIg,x,y)*d;
        g = g*c + f*d;
        IG_UPUT(pIg,x,y,g);
       }

     /* right-to-left */
      for (x=pIg->width()-1; x>=0; x--) {
        f = f*c + IG_UGET(pIg,x,y)*d;
        g = g*c + f*d;
        IG_UPUT(pIg,x,y,g);
      }

    /* left-to-right mop-up */
      for (x=0; x<pIg->width(); x++) {
        f = f*c;
        g = g*c + f*d;
        if (f+g < 1/255.0) {
           break;
      }
      double d=IG_UGET(pIg,x,y)+g;
      IG_UPUT(pIg,x,y,d);
    }
  }
  for (x=0; x<pIg->width(); x++) {
     /* apply low-pass filter to column x */
     /* bottom-to-top */
     f = g = 0;
     for (y=0; y<pIg->height(); y++) {
       f = f*c + IG_UGET(pIg,x,y)*d;
       g = g*c + f*d;
       IG_UPUT(pIg,x,y,g);
     }

     /* top-to-bottom */
     for (y=pIg->height()-1; y>=0; y--) {
       f = f*c + IG_UGET(pIg,x,y)*d;
       g = g*c + f*d;
       IG_UPUT(pIg,x,y,g);
     }

     /* bottom-to-top mop-up */
     for (y=0; y<pIg->height(); y++) {
       f = f*c;
       g = g*c + f*d;
       if (f+g < 1/255.0) {
           break;
       }
       double d = IG_UGET(pIg,x,y)+g;
       IG_UPUT(pIg,x,y,d);
     }
   }
  return true;
 }


/* apply highpass filter to greymap. Return 0 on success, 1 on error
   with errno set. */
bool Tracer::highpass(QImage *pIg, double lambda) {
  double f;
  int x, y;
  if(!pIg){
      return false;
  }
  if (pIg->width() == 0 ||pIg->height() == 0) {
    return false;
  }

  /* create a copy */
  QImage ig1;
  ig1 = pIg->copy(0,0,pIg->width(),pIg->height());

  /* apply lowpass filter to the copy */
  lowpass(&ig1, lambda);

  /* subtract copy from original */
  int g;
  //use scanline //OK
    for (y=0; y<pIg->height(); y++) {
      for (x=0; x<pIg->width(); x++) {
        f = IG_UGET(pIg,x,y);
        f -= IG_UGET(&ig1,x,y);
        f += 128;    /* normalize! */
        IG_UPUT(pIg,x,y,f);
      }
    }
  return true;
}
bool Tracer::threshold(QImage *pIg,double c)
{
    int w=0, h=0;
    double c1;
    int x, y;
    w=pIg->width();
    h=pIg->height();
    if(w==0,h==0){
        return false;
    }
     /* thresholding */
    c1 = c * 255;
    double g;
    for (y=0; y<h; y++) {
      for (x=0; x<w; x++) {
          g = IG_UGET(pIg,x,y);
          if(g>=c1){
             g=255;
          }else{
             g=0;
          }
        IG_UPUT(pIg,x,y,g);
       }
     }
    return true;
}
potrace_state_t * Tracer::traceToPath( potrace_param_t *param,potrace_bitmap_t *bm)
{
    potrace_state_t *st;

    if(!bm){
        return 0;
    }
    if(!param) {
        qDebug()<<"param is empyt";
        return 0;
    }

    st = potrace_trace(param, bm);
    if (!st || st->status != POTRACE_STATUS_OK) {
         qDebug()<<"st is empyt";
         return 0;
    }
    return st;
}
