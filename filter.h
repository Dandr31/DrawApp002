#ifndef FILTER_H
#define FILTER_H
#include <potrace/potracelib.h>
#include <potrace/bitmap.h>
#include <QImage>

/*macros for writing QImage */
#define ig_index(ig,x,y)((ig)->scanLine(y)+(x))
#define IG_UGET(ig,x,y)(*ig_index(ig,x,y))
#define IG_UPUT(ig, x, y, b) (*ig_index(ig, x, y) = (signed short int)(b))

class Filter
{
public:
    enum filterType {EDGEDECTION,BITMAP, BRIGHTNESS};
    Filter();

    virtual QImage * filters(QImage *pImage);

};
class MakeBitmap:public Filter
{
public:
    MakeBitmap(){
        m_c=0.45;
        m_lambda =4;
    }

   QImage * filters(QImage *pImage)override;

   bool  threshold(QImage *pIg,double c);

   bool  highpass(QImage *pIg, double lambda=4);

   bool lowpass(QImage *pIg, double lambda=4);

   double getC(){
       return m_c;
   }
   void setC(double c){
       m_c = c;
   }

   double getLambda(){
       return m_lambda;
   }
   void setLambda(double lambda)
   {
       m_lambda = lambda;
   }


private:
   double m_c;
   double m_lambda;

};
class EdgeDection:public Filter
{
public:
    EdgeDection(){
         m_lowThreshold =0.1;
         m_highThreshold=0.65;
    }

    QImage * filters(QImage *pImage)override;

    /*functions about edge detection*/
    static QImage * grayMapSobel(QImage *pImage,
                   double dLowThreshold, double dHighThreshold);

    static QImage *grayMapCanny(QImage *pImage, double lowThreshold, double highThreshold);

    double getHighTrheshold(){
        return m_highThreshold;
    }

    void  setHighTrheshold(double ht){
        m_highThreshold =ht;
    }

    double getLowThreshold(){
        return m_lowThreshold;
    }

    void setLowThreshold(double lt){
        m_lowThreshold =lt;
    }

private:
    double m_lowThreshold;
    double m_highThreshold;
};
class Brightness:public Filter
{
public:
    Brightness(){
       m_floor = 0.1;
       m_cutoff = 0.45;
    }
     QImage * filters(QImage *pImage)override;

     double getFloor(){
         return m_floor;
     }
     void setFloor(double floor){
         m_floor =  floor;
     }

     double getCutoff()
     {
         return m_cutoff;
     }
     void setCutoff(double cutoff){
         m_cutoff = cutoff;
     }
private:
     double m_floor;
     double m_cutoff;
};

#endif // FILTER_H
