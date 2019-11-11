#ifndef IMAGEPROCESS_H
#define IMAGEPROCESS_H
#include <QImage>
#include <QSvgRenderer>
bool rangeCheck(double f);
QRgb grayToRgb(double f);
int  process_threshold(QImage *pIg,double c);

int  process_highpass(QImage *pIg, double lambda);

void process_lowpass(QImage *pIg, double lambda);

QSvgRenderer * process_svgResizeTo(const QString &fileName,QSize size);

QSvgRenderer * process_imageToSvg(const QString &fileName);

QString replaceSuffix(QString str,QString suf);

QString getSuffix(QString str);

QString makeBitmap(const QString &fileName);

QString potrace(const QString &fileName);
#endif // IMAGEPROCESS_H
