#ifndef TRACER_H
#define TRACER_H
/*macros for writing image object */
#define ig_index(ig,x,y)((ig)->scanLine(y)+(x))
#define IG_UGET(ig,x,y)(*ig_index(ig,x,y))
#define IG_UPUT(ig, x, y, b) (*ig_index(ig, x, y) = (signed short int)(b))
/* macros for writing individual bitmap pixels */
#define BM_WORDSIZE ((int)sizeof(potrace_word))
#define BM_WORDBITS (8*BM_WORDSIZE)
#define BM_HIBIT (((potrace_word)1)<<(BM_WORDBITS-1))
#define bm_scanline(bm, y) ((bm)->map + (y)*(bm)->dy)
#define bm_index(bm, x, y) (&bm_scanline(bm, y)[(x)/BM_WORDBITS])
#define bm_mask(x) (BM_HIBIT >> ((x) & (BM_WORDBITS-1)))
#define bm_range(x, a) ((int)(x) >= 0 && (int)(x) < (a))
#define bm_safe(bm, x, y) (bm_range(x, (bm)->w) && bm_range(y, (bm)->h))
#define BM_UGET(gm, x, y) (*bm_index(bm, x, y))
#define BM_USET(bm, x, y) (*bm_index(bm, x, y) |= bm_mask(x))
#define BM_UCLR(bm, x, y) (*bm_index(bm, x, y) &= ~bm_mask(x))
#define BM_UPUT(bm, x, y, b) ((b) ? BM_USET(bm, x, y) : BM_UCLR(bm, x, y))
#define BM_PUT(bm, x, y, b) (bm_safe(bm, x, y) ? BM_UPUT(bm, x, y, b) : 0)

#include <potrace/potracelib.h>
#include <QString>
#include <QImage>
class Tracer
{
public:
    Tracer();

    inline QString toQString(double t);

    bool traceToSvg(QImage *pImage,const QString &filename);

    void test2();

    void test();

    /* functions to make bitmap*/
    static potrace_bitmap_t* mkbitmap(QImage *pImage,double c);

    static  bool  threshold(QImage *pIg,double c);

    static  bool  highpass(QImage *pIg, double lambda=4);

    static  bool lowpass(QImage *pIg, double lambda=4);
    /*functions about image and bm*/
    //svae the bm
    bool write_bm2(const QString &fileName,potrace_bitmap_t *bm);
    //QFile will produce some bug
    bool write_bm(const QString &fileName,potrace_bitmap_t *bm);

    static  potrace_bitmap_t* imageToBitmap(QImage *pImage);

    static potrace_bitmap_t* bm_new(int w, int h);

    static void bm_free(potrace_bitmap_t *bm);

    /*functions about edge detection*/
    static QImage * grayMapSobel(QImage *pImage,
                   double dLowThreshold, double dHighThreshold);

    static QImage *grayMapCanny(QImage *pImage, double lowThreshold, double highThreshold);

    static bool getInvertBm(potrace_bitmap_t *bm);
    /*functions about trace bm to path*/

    potrace_state_t * traceToPath( potrace_param_t *param,potrace_bitmap_t *bm);


private:

};

#endif // TRACER_H
