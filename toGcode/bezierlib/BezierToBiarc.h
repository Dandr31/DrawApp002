//
//  Algorithm.h
//  BezierToBiarc
//
//  Created by Carsten Høyer on 31/01/2019.
//

#pragma once

#include "../dpoint.h"
#include "CubicBezier.h"
#include <complex>
#include <tuple>
#include <stack> 
#include "BiArc.h"
typedef std::vector<BiArc> BiarcVector;
class BezierToBiarc {
public:
    BezierToBiarc();
	BiarcVector ApproxCubicBezier(CubicBezier bezier, float samplingStep, float tolerance);
private:
    static bool IsRealInflexionPoint(complex<float> t);
    DPoint bezier_at_t( CubicBezier bez ,float t);
    double  BezierToBiarc::point_to_arc_distance(DPoint p,Arc arc);
    bool  BezierToBiarc::between(double c,double x,double y);
    double straight_tolerance = 0.01;
};
