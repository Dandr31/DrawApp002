#pragma once
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include "SVGParser.h"
#include "dpoint.h"
#include "bezierlib/CubicBezier.h"
#include "bezierlib/BezierToBiarc.h"
#include "gcode.h"
#include <math.h>
#include <QPointF>
#include <QTransform>
#define D_Pi 3.1415926
using namespace SVGParser;
using namespace Gcode;
class GcodeGenerator
{
public:
	enum GcodeType {
		GCODE_NORMAL,
		GCODE_LASER
	};
	GcodeGenerator();

    bool goGcode(const char* str, const char* filename,QTransform qtransform);

	SVGPath  parsePath(const char* str);

    inline QPointF toQPointF(DPoint dpoint);

    inline DPoint toDPoint(QPointF qpoint);

    SVGPath transformPath(SVGPath svgPath,QTransform qtransform);

	GcodeArcTo* biarcToGcode(Arc arc);

	GcodePath  convertToGcode(SVGPath svgPath);

	virtual void  setGcodeType(GcodeType type);

	virtual GcodeType getType();
	
private:
	GcodeType m_type;
};

