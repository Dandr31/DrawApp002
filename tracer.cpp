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
  setFilter(1);
}
//this function need to change
void Tracer::setFilter(int n)
{
    if(n == Filter::EDGEDECTION){
        m_filter = new EdgeDection();
        return;
    }
    if(n==Filter::BITMAP){
        m_filter = new MakeBitmap();
        return;
    }
    if(n==Filter::BRIGHTNESS){
        m_filter = new Brightness();
        return;
    }
    m_filter = new EdgeDection();
}
bool Tracer::traceToSvg(QImage *pImage,const QString &filename)
{
    potrace_bitmap_t *bm;
    potrace_param_t *param;
    potrace_state_t *st;

    QImage image = pImage->convertToFormat(QImage::Format_Grayscale8);

    bm = imageToBitmap(m_filter->filters(&image));
//    getInvertBm(bm);
    if(!bm){
        qDebug()<<"bm==0";
        return 0;
    }
    /* set tracing parameters, starting from defaults */
    param = potrace_param_default();
    if (!param) {
      return 0;
    }
//    param->turdsize = 0;

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

void Tracer::test2()
{

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
//    bm = mkbitmap(pImage,0.45);
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
    //is the y-axis symmetric graphic of the original image
    for (y=0; y<height; y++) {
      for (x=0; x<width; x++) {
        //inverted the direction and color of  pImage
        BM_PUT(bm, x, y,255-IG_UGET(pImage,x,y));
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

bool Tracer::getInvertBm(potrace_bitmap_t *bm)
{
    if(!bm)
        return false;

     for (int y=0 ; y<bm->h ; y++){
         for (int x=0 ; x<bm->w ; x++){
                   int brightness = BM_UGET(bm, x, y);
                   if(y<1){
                       qDebug()<<brightness;
                   }
//                   brightness = 255 - brightness;
//                   BM_UPUT(bm, x, y, brightness);
         }
     }
     return true;
}
