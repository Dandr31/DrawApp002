#ifndef TRACER_H
#define TRACER_H
/*macros for writing image object */
#define ig_index(ig,x,y)((ig)->scanLine(y)+(x))
#define IG_UGET(ig,x,y)(*ig_index(ig,x,y))
#define IG_UPUT(ig, x, y, b) (*ig_index(ig, x, y) = (signed short int)(b))
#include <potrace/bitmap.h>
#include <potrace/potracelib.h>
#include <QString>
#include <QImage>
#include "filter.h"
class Tracer
{
public:

    Tracer();

    inline QString toQString(double t);

    bool traceToSvg(QImage *pImage,const QString &filename);

    void test2();

    void test();

    /*functions about image and bm*/
    //svae the bm
    bool write_bm2(const QString &fileName,potrace_bitmap_t *bm);
    
    //QFile will produce some bug
    bool write_bm(const QString &fileName,potrace_bitmap_t *bm);

    static  potrace_bitmap_t* imageToBitmap(QImage *pImage);


    static bool getInvertBm(potrace_bitmap_t *bm);
    /*functions about trace bm to path*/

    potrace_state_t * traceToPath( potrace_param_t *param,potrace_bitmap_t *bm);

    void setFilter(int n);


private:
    Filter  *m_filter;
};

#endif // TRACER_H
