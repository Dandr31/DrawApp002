//
//  Algorithm.cpp
//  BezierToBiarc
//
//  Created by Carsten Høyer on 31/01/2019.
//

#include "BezierToBiarc.h"
#include <QDebug>
#define Pi 3.1415926

BezierToBiarc::BezierToBiarc() {
    
}

BiarcVector  BezierToBiarc::ApproxCubicBezier(CubicBezier bezier, float samplingStep, float tolerance) {
    vector<BiArc> biarcs;
    stack<CubicBezier> curves;
    qDebug()<<QString::fromStdString(bezier.toString());

    // ---------------------------------------------------------------------------
    // First, calculate the inflexion points and split the bezier at them (if any)
    
    CubicBezier toSplit = bezier;
    auto inflex = toSplit.InflexionPoints();
    bool i1 = IsRealInflexionPoint(get<0>(inflex));
    bool i2 = IsRealInflexionPoint(get<1>(inflex));
    
    if (i1 && !i2)
    {
        auto splited = toSplit.Split(real(get<0>(inflex)));
        curves.push(get<1>(splited));
        curves.push(get<0>(splited));
    }
    else if (!i1 && i2)
    {
        auto splited = toSplit.Split(real(get<1>(inflex)));
        curves.push(get<1>(splited));
        curves.push(get<0>(splited));
    }
    else if (i1 && i2)
    {
        float t1 = real(get<0>(inflex));
        float t2 = real(get<1>(inflex));
        
        // I'm not sure if I need, but it does not hurt to order them
        if (t1 > t2)
        {
            auto tmp = t1;
            t1 = t2;
            t2 = tmp;
        }
        
        // Make the first split and save the first new curve. The second one has to be splitted again
        // at the recalculated t2 (it is on a new curve)
        
        auto splited1 = toSplit.Split(t1);
        
        t2 = (1 - t1) * t2;
        
        toSplit = get<1>(splited1);
        auto splited2 = toSplit.Split(t2);
        
        curves.push(get<1>(splited2));
        curves.push(get<0>(splited2));
        curves.push(get<0>(splited1));
    }
    else {
        curves.push(toSplit);
    }
    
   // int count =0;//a hack to avoid  infinite loop
   // int maxCount =20;
    // ---------------------------------------------------------------------------
    // Second, approximate the curves until we run out of them
    while (curves.size() > 0)
    {
       //  count++;
       // if(count>maxCount){
       //     break;
       // }
        CubicBezier bezier = curves.top();
        curves.pop();
        
        // ---------------------------------------------------------------------------
        // Calculate the transition DPoint for the BiArc
        // V: Intersection DPoint of tangent lines
        Line T1 = Line(bezier.P1, bezier.C1);
        Line T2 = Line(bezier.P2, bezier.C2);
        
        // I think this is a hack for an underlying problem.
        // If the lines are parallel on the x axis, we cannot get the incenter, because we cannot create
        // a triangle. So in order to avoid this, we rotate the tangent just a little bit.
        if (isnan(T1.m)) {
            DPoint skewedPoint1 = DPoint(bezier.C1.m_x - 0.001, bezier.C1.m_y);
            T1 = Line(bezier.P1, skewedPoint1);
        }
        if (isnan(T2.m)) {
            DPoint skewedPoint2 = DPoint(bezier.C2.m_x - 0.001, bezier.C2.m_y);
            T2 = Line(bezier.P2, skewedPoint2);
        }
        DPoint V = T1.Intersection(T2);
        
        // G: incenter DPoint of the triangle (P1, V, P2)
        // http://www.mathopenref.com/coordincenter.html
        float dP2V = bezier.P2.distance(V);
        float dP1V = bezier.P1.distance(V);
        float dP1P2 = bezier.P1.distance(bezier.P2);
        DPoint G = (dP2V * bezier.P1 + dP1V * bezier.P2 + dP1P2 * V) / (dP2V + dP1V + dP1P2);
        
        // ---------------------------------------------------------------------------
        // Calculate the BiArc
        
        BiArc biarc = BiArc(bezier.P1, (bezier.P1 - bezier.C1), bezier.P2, (bezier.P2 - bezier.C2), G);
        
        // ---------------------------------------------------------------------------
        // Calculate the maximum error
        
        float maxDistance = 0.0f;
        float maxDistanceAt = 0.0f;
        
        auto nrPointsToCheck = biarc.Length() / samplingStep;
        auto parameterStep = 1.0f / nrPointsToCheck;

        for (int i = 0; i <= nrPointsToCheck; i++)
        {
            //cout << "check: " << i << "\n";
            float t = parameterStep * i;
            DPoint u1 = biarc.PointAt(t);
            DPoint u2 = bezier.PointAt(t);
            float distance = (u1 - u2).length();
            qDebug()<<"distance"<<distance;
            if (distance > maxDistance)
            {
                maxDistance = distance;
                maxDistanceAt = t;
            }
            /*
            float t = parameterStep*i;
            DPoint u2 =bezier_at_t(bezier,t);
            float d1 =point_to_arc_distance(u2,biarc.A1);
            float d2 =point_to_arc_distance(u2,biarc.A2);
            if(d1>d2)
                d1=d2;
            if(d1>maxDistance){
                maxDistance = d1;
                maxDistanceAt = t;
            }*/
        }
        qDebug()<<"maxDistance"<<maxDistance;
        // Check if the two curves are close enough
        if (maxDistance > tolerance)
        {
            // If not, split the bezier curve the DPoint where the distance is the maximum
            // and try again with the two halfs
            auto bs = bezier.Split(maxDistanceAt);
            curves.push(get<1>(bs));
            curves.push(get<0>(bs));
        }
        else
        {
            // Otherwise we are done with the current bezier
            biarcs.push_back(biarc);
        }
    }
    qDebug()<<"ApproxCubicBezier end";
	return biarcs;
}

bool BezierToBiarc::IsRealInflexionPoint(complex<float> t)
{
    return imag(t) == 0 && real(t) > 0 && real(t) < 1;
}


DPoint  BezierToBiarc::bezier_at_t( CubicBezier bez ,float t)
{
   double  ax =bez.P1.m_x;
   double  bx =bez.C1.m_x;
   double  cx =bez.C2.m_x;
   double  dx =bez.P2.m_x;


   double  ay =bez.P1.m_y;
   double  by =bez.C1.m_y;
   double  cy =bez.C2.m_y;
   double  dy =bez.P2.m_y;

   double  x1 =ax+(bx-ax)*t;
   double  y1 =ay+(by-ay)*t;

   double  x2 =bx+(cx-bx)*t;
   double  y2 =by+(cy-by)*t;
   
   double  x3 =cx+(dx-cx)*t;
   double  y3 =cy+(dy-cy)*t;
   
   double  x4 = x1 +(x2-x1)*t;
   double  y4 = y1 +(y2-y1)*t;

   double  x5 = x2 +(x3-x2)*t;
   double  y5 = y2 +(y3-y2)*t;
   
   double x = x4+(x5-x4)*t;
   double y = y4+(y5-y4)*t;

   return DPoint(x,y);
}
double  BezierToBiarc::point_to_arc_distance(DPoint p,Arc arc)
{
   DPoint P0 = arc.P1;
   DPoint P2 = arc.P2;
   DPoint C  = arc.C;
   double a = arc.sweepAngle;
   double r = (P0-C).mag();

   if(r>0){
       DPoint i = C + (P0-C).unit()*r;
       double alpha = ((i-C).angle()-(P0-C).angle());
       if(a*alpha <0){
           if(alpha>0){
               alpha = alpha-Pi*Pi;
           }else{
               alpha = alpha+Pi*Pi;
           }//if alpha>0 end
       }//if a* alpha end
       if(between(alpha,0,a)||min(abs(alpha),abs(alpha-a))<straight_tolerance){

		return (p-i).mag();
       }else{
           double  d1 =(p-P0).mag();
           double  d2 =(p-P2).mag();
           if(d1<d2){
                return  d1;
           }else{
                return  d2;
           }

       }//if(between(alpha,0,....end

   }//if r>0 end
	
}
bool  BezierToBiarc::between(double c,double x,double y){

     return( x-straight_tolerance<=c&&c<=y+straight_tolerance || y-straight_tolerance<=c&&c<=x+straight_tolerance);
}
