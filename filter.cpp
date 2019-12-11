#include "filter.h"
#include <QDebug>
Filter::Filter()
{

}
QImage * Filter::filters(QImage *pImage)
{
    if(!pImage){
        return NULL;
    }
    QImage *newImage = new QImage(pImage->width(),pImage->height(),pImage->format());
    return newImage;
}
/*#########################################################################
### MAKE BITMAP FROM POTRACE
#########################################################################*/

/* apply lowpass filter (an approximate Gaussian blur) to greymap.
   Lambda is the standard deviation of the kernel of the filter (i.e.,
   the approximate filter radius). */
bool MakeBitmap::lowpass(QImage *pIg, double lambda) {
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
bool MakeBitmap::highpass(QImage *pIg, double lambda) {
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
bool MakeBitmap::threshold(QImage *pIg,double c)
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

QImage * MakeBitmap::filters(QImage *pImage)
{

   if(!pImage)
            return 0;
   if(pImage->width()==0||pImage->height()==0)
            return 0;
   QImage image;

   image = pImage->convertToFormat(QImage::Format_Grayscale8);


   if(!highpass(&image))
                return 0;
   if(!threshold(&image,m_c))
                return 0;
   QImage *new_pImage = new QImage(image);

   return new_pImage ;



}
/*#########################################################################
### C A N N Y    E D G E    D E T E C T I O N
#########################################################################*/


static int sobelX[] =
{
    -1,  0,  1 ,
    -2,  0,  2 ,
    -1,  0,  1
};

static int sobelY[] =
{
     1,  2,  1 ,
     0,  0,  0 ,
    -1, -2, -1
};



/**
 * Perform Sobel convolution on a GrayMap
 */
QImage * EdgeDection::grayMapSobel(QImage *pImage,
               double dLowThreshold, double dHighThreshold)
{
    int width  = pImage->width();
    int height = pImage->height();
    int firstX = 1;
    int lastX  = width-2;
    int firstY = 1;
    int lastY  = height-2;

    QImage *newIg = new QImage(width,height,QImage::Format_Grayscale8);
    if (!newIg)
        return NULL;

    for (int y = 0 ; y<height ; y++)
        {
        for (int x = 0 ; x<width ; x++)
            {
            unsigned long sum = 0;
        /* image boundaries */
            if (x<firstX || x>lastX || y<firstY || y>lastY)
                {
                sum = 0;
                }
            else
                {
                /* ### SOBEL FILTERING #### */
                long sumX = 0;
                long sumY = 0;
                int sobelIndex = 0;
                for (int i= y-1 ; i<=y+1 ; i++)
                    {
                    for (int j= x-1; j<=x+1 ; j++)
                        {
                        sumX += IG_UGET(pImage, j, i) *
                             sobelX[sobelIndex++];
                }
                }

                sobelIndex = 0;
                for (int i= y-1 ; i<=y+1 ; i++)
                    {
                    for (int j= x-1; j<=x+1 ; j++)
                        {
                        sumY += IG_UGET(pImage, j, i) *
                             sobelY[sobelIndex++];
                }
                }
                /*###  GET VALUE ### */
                sum = abs(sumX) + abs(sumY);

                if (sum > 255)
                    sum = 255;

                /*###  GET EDGE DIRECTION (fast way) ### */
                int edgeDirection = 0; /*x,y=0*/
                if (sumX==0)
                    {
                    if (sumY!=0)
                        edgeDirection = 90;
                    }
                else
                   {
                   /*long slope = sumY*1024/sumX;*/
                   long slope = (sumY << 10)/sumX;
                   if (slope > 2472 || slope< -2472)  /*tan(67.5)*1024*/
                       edgeDirection = 90;
                   else if (slope > 414) /*tan(22.5)*1024*/
                       edgeDirection = 45;
                   else if (slope < -414) /*-tan(22.5)*1024*/
                       edgeDirection = 135;
                   }

                /* printf("%ld %ld %f %d\n", sumX, sumY, orient, edgeDirection); */

                /*### Get two adjacent pixels in edge direction ### */
                unsigned long leftPixel;
                unsigned long rightPixel;
                if (edgeDirection == 0)
                    {
                    leftPixel  = IG_UGET(pImage, x-1, y);
                    rightPixel = IG_UGET(pImage, x+1, y);
                    }
                else if (edgeDirection == 45)
                    {
                    leftPixel  = IG_UGET(pImage, x-1, y+1);
                    rightPixel = IG_UGET(pImage, x+1, y-1);
                    }
                else if (edgeDirection == 90)
                    {
                    leftPixel  = IG_UGET(pImage, x, y-1);
                    rightPixel = IG_UGET(pImage, x, y+1);
                    }
                else /*135 */
                    {
                    leftPixel  = IG_UGET(pImage, x-1, y-1);
                    rightPixel = IG_UGET(pImage, x+1, y+1);
                    }

                /*### Compare current value to adjacent pixels ### */
                /*### if less that either, suppress it ### */
                if (sum < leftPixel || sum < rightPixel)
                    sum = 0;
                else
                    {
                    unsigned long highThreshold =
                          (unsigned long)(dHighThreshold * 255.0);
                    unsigned long lowThreshold =
                          (unsigned long)(dLowThreshold * 255.0);
                    if (sum >= highThreshold)
                        sum = 255; /* EDGE.  3*255 this needs to be settable */
                    else if (sum < lowThreshold)
                        sum = 0; /* NONEDGE */
                    else
                        {
                        if ( IG_UGET(pImage, x-1, y-1)> highThreshold ||
                             IG_UGET(pImage, x  , y-1)> highThreshold ||
                             IG_UGET(pImage, x+1, y-1)> highThreshold ||
                             IG_UGET(pImage, x-1, y  )> highThreshold ||
                             IG_UGET(pImage, x+1, y  )> highThreshold ||
                             IG_UGET(pImage, x-1, y+1)> highThreshold ||
                             IG_UGET(pImage, x  , y+1)> highThreshold ||
                             IG_UGET(pImage, x+1, y+1)> highThreshold)
                            sum = 255; /* EDGE fix me too */
                        else
                            sum = 0; /* NONEDGE */
                        }
                    }


                }/* else */
            if (sum==0) /* invert light & dark */
                sum = 255;
            else
                sum = 0;
           IG_UPUT(newIg, x, y, sum);
        }/* for (x) */
    }/* for (y) */

    return newIg;
}


/**
 *
 */
QImage *EdgeDection::grayMapCanny(QImage *pImage, double lowThreshold, double highThreshold)
{
    if (!pImage)
        return NULL;

    QImage *cannyImage = grayMapSobel(pImage, lowThreshold, highThreshold);
    if (!cannyImage)
        return NULL;
    /*cannyGm->writePPM(cannyGm, "canny.ppm");*/

    return cannyImage;
}


QImage * EdgeDection::filters(QImage *pImage)
{
    if(!pImage)
        return NULL;
     return grayMapCanny(pImage,m_lowThreshold,m_highThreshold);
}

QImage * Brightness::filters(QImage *pImage)
{
    if(!pImage)
        return false;
    QImage *newGrayImg =new QImage(pImage->width(),pImage->height(),QImage::Format_Grayscale8);
    double floor = 3.0 *m_floor*256.0;
    double cutoff =3.0 *m_cutoff*256.0;
    for (int y=0 ; y<pImage->height() ; y++)
     {
        for (int x=0 ; x<pImage->width() ; x++)
        {
               double brightness =(double)IG_UGET(pImage,x,y);
//             double brightness = (double)gm->getPixel(gm, x, y);
              if (brightness >= floor && brightness < cutoff)
              {
                  IG_UPUT(newGrayImg,x,y,255);
//                      newGm->setPixel(newGm, x, y, GRAYMAP_BLACK);  //black pixel
              }else{
                  IG_UPUT(newGrayImg,x,y,0);
//            newGm->setPixel(newGm, x, y, GRAYMAP_WHITE); //white pixel
              }

         }
    }
    return  newGrayImg;

}
