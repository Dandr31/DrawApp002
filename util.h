#ifndef IMAGEPROCESS_H
#define IMAGEPROCESS_H
#include <QImage>
#include <QSvgRenderer>
#define ig_index(ig,x,y)((ig)->scanLine(y)+(x))
#define IG_UGET(ig,x,y)(*ig_index(ig,x,y))
#define IG_UPUT(ig, x, y, b) (*ig_index(ig, x, y) = (signed short int)(b))
bool rangeCheck(double f);

QRgb grayToRgb(double f);

int  process_threshold(QImage *pIg,double c);

int  process_highpass(QImage *pIg, double lambda=4);

void process_lowpass(QImage *pIg, double lambda=4);

QSvgRenderer * process_svgResizeTo(const QString &fileName,QSize size);

QSvgRenderer * process_imageToSvg(const QString &fileName);

QString replaceSuffix(QString str,QString suf);


QString getSuffix(QString str);

QSizeF suitableSize(QSizeF cur_size,QSizeF size);

QString makeBitmapByPotrace(const QString &fileName);

QString makeBitmap(const QString &fileName,double c=0.45);

QString potrace(const QString &fileName);

bool generateGcode(const QString &fileName);
#endif // IMAGEPROCESS_H
