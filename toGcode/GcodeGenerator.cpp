#include "GcodeGenerator.h"
#include <QDebug>
GcodeGenerator::GcodeGenerator():m_type(GCODE_NORMAL)
{
}

void GcodeGenerator::setGcodeType(GcodeType type)
{
	m_type = type;
}

GcodeGenerator::GcodeType GcodeGenerator::getType()
{
	return m_type;
}
Gcode::GcodeArcTo* GcodeGenerator::biarcToGcode(Arc arc) {
	//Arc(DPoint _C, float _r, float _startAngle, float _sweepAngle, DPoint _P1, DPoint _P2, bool _cw)
	//GcodeArcTo::GcodeArcTo(float rx, float ry,float x0,float y0, bool cw, float x, float y,float z)

	float x0 = arc.P1.m_x;
	float y0 = arc.P1.m_y;

	float x1 = arc.P2.m_x;
	float y1 = arc.P2.m_y;
    /*
	std::cout << arc.r << endl;
	std::cout << arc.P1.m_x << "\t" << arc.P1.m_y << "\t" << arc.P1.distance(arc.C) << endl;
	std::cout << arc.P2.m_x << "\t" << arc.P2.m_y << "\t" << arc.P2.distance(arc.C) << endl;
	std::cout << endl;
	*/

	return new Gcode::GcodeArcTo(arc.C.m_x, arc.C.m_y, x0, y0, arc.cw, x1, y1, 0);
}
Gcode::GcodePath GcodeGenerator::convertToGcode(SVGParser::SVGPath svgPath)
{
	BezierToBiarc convert;
	Gcode::GcodePath gPath;
	bool isLast = false;
	int subpath = -1;
	DPoint subpathstart;
	DPoint lastctrlPoint;
	DPoint lastPoint;
	for (SVGPath::iterator it = svgPath.begin(); it != svgPath.end(); it++) {
		SVGCommand* cmd = *it;
		switch (cmd->getType()) {
		case SVG_MOVE_TO_ABS:
			gPath.push_back(new Gcode::GcodeMoveTo(cmd->x, cmd->y, 0));

			lastctrlPoint = DPoint(cmd->x, cmd->y);
			lastPoint = DPoint(cmd->x, cmd->y);
			subpathstart = DPoint(cmd->x, cmd->y);
			isLast = true;
			break;
		case SVG_LINE_TO_ABS:
		{
			gPath.push_back(new Gcode::GcodeLineTo(cmd->x, cmd->y, 0));
			lastctrlPoint = DPoint(cmd->x, cmd->y);
			lastPoint = DPoint(cmd->x, cmd->y);
			isLast = true;
		}
		break;
		case SVG_CUBIC_CURVE_TO_ABS:
		{
			DPoint p1 = lastPoint;
			DPoint p2 = DPoint(((SVGCubicCurveTo*)cmd)->x0, ((SVGCubicCurveTo*)cmd)->y0);
			DPoint p3 = DPoint(((SVGCubicCurveTo*)cmd)->x1, ((SVGCubicCurveTo*)cmd)->y1);
			DPoint p4 = DPoint(cmd->x, cmd->y);
			CubicBezier c(p1, p2, p3, p4);
			BiarcVector biarcs = convert.ApproxCubicBezier(c, 0.5, 0.1);
			int mode = 2;//this is a hack to avoid generate circle
			gPath.push_back(new Gcode::GcodeComment(c.toString()));
			for (BiarcVector::const_iterator b_it = biarcs.begin(); b_it != biarcs.end(); b_it++) {
				if (mode == 0) {
					/*G02 or G03*/
					gPath.push_back(biarcToGcode((*b_it).A1));
					gPath.push_back(biarcToGcode((*b_it).A2));
				}
				else if (mode == 1) {
					/*all in G01*/
					Gcode::GcodePath gp1 = Gcode::arcToLines(*biarcToGcode((*b_it).A1));
					for (Gcode::GcodePath::const_iterator g1_it = gp1.begin(); g1_it != gp1.end(); g1_it++) {
						gPath.push_back(*g1_it);
					}

					Gcode::GcodePath gp2 = Gcode::arcToLines(*biarcToGcode((*b_it).A2));
					for (Gcode::GcodePath::const_iterator g2_it = gp2.begin(); g2_it != gp2.end(); g2_it++) {
						gPath.push_back(*g2_it);
					}
				}
				else if (mode == 2) {

					gPath.push_back(new Gcode::GcodeLineTo((*b_it).A1.P1.m_x, (*b_it).A1.P1.m_y, 0));
					gPath.push_back(new Gcode::GcodeLineTo((*b_it).A1.P2.m_x, (*b_it).A1.P2.m_y, 0));
					gPath.push_back(new Gcode::GcodeLineTo((*b_it).A2.P1.m_x, (*b_it).A2.P1.m_y, 0));
				}


			}
			lastPoint = DPoint(cmd->x, cmd->y);
			lastctrlPoint = DPoint(((SVGCubicCurveTo*)cmd)->x1, ((SVGCubicCurveTo*)cmd)->y1);
			isLast = true;
		}
		//case SVG_CLOSE_PATH:
			//lastPoint = subpathstart;
		//	lastctrlPoint = subpathstart;
		//	isLast = true;*/
		}//switch end

	}//for end
	return gPath;
}
SVGPath GcodeGenerator::parsePath(const char* str)
{
	std::istringstream pathStream(str);
	SVGParser::SVGPath svgPath = SVGParser::parsePath(pathStream);

	// SVGParser::SVGPath absPath = SVGParser::toAbsolute(svgPath);//this is not correct
	/*
	std::ofstream relFile;
	relFile.open("rel1212.txt");
	for (SVGParser::SVGPath::const_iterator s_it = svgPath.begin(); s_it != svgPath.end(); s_it++) {
		relFile << (*s_it) << endl;
	}
	relFile.close();*/

	SVGParser::SVGPath absPath2 = SVGParser::toAbs(svgPath);
	
	/*
	std::ofstream absFile("abs121.txt");
	for (SVGParser::SVGPath::const_iterator a_it = absPath2.begin(); a_it != absPath2.end(); a_it++) {
		absFile << (*a_it) << endl;
	}
	absFile.close();*/


	return absPath2;
}
bool GcodeGenerator::goGcode(const char* str, const char* filename,QTransform qtransform)
{
	if (!str) {
		return false;
	}
	SVGPath svgPath =parsePath(str);
/*
    SVGPath tranformedSvgPath = transformPath(svgPath,qtransform);
    qDebug()<<"transform path has  end";
    std::ofstream testFile;
    testFile.open("C:/Users/Thinkpad/Pictures/gcodevs/rel1212.txt");
    for (SVGPath::iterator it = tranformedSvgPath.begin(); it != tranformedSvgPath.end(); it++) {
       testFile << (*it) << endl;
    }
    testFile.close();

    std::ofstream trasFile;
    trasFile.open("C:/Users/Thinkpad/Pictures/gcodevs/trans.txt");
    for (SVGPath::iterator t_it = svgPath.begin(); t_it != svgPath.end(); t_it++) {
       trasFile << (*t_it) << endl;
    }
    trasFile.close();
    */
    Gcode::GcodePath gPath = convertToGcode(svgPath);

	std::ofstream outfile;

	outfile.open(filename);
	if (m_type == GCODE_NORMAL) {
		for (Gcode::GcodePath::const_iterator g_it = gPath.begin(); g_it != gPath.end(); g_it++) {
			outfile << (*g_it)->toGcode() << endl;
		}
	}
	else if (m_type == GCODE_LASER) {
		
	}
	
	outfile.close();

}
inline QPointF GcodeGenerator::toQPointF(DPoint dpoint){
    return QPointF(dpoint.m_x,dpoint.m_y);
}

inline DPoint GcodeGenerator::toDPoint(QPointF qpoint){
    return DPoint(qpoint.x(),qpoint.y());
}

SVGPath GcodeGenerator::transformPath(SVGPath svgPath,QTransform qtransform){
    SVGPath new_path;
    qDebug()<<"transfrom"<<qtransform;
    for (SVGPath::iterator it = svgPath.begin(); it != svgPath.end(); it++) {
             SVGCommand* cmd = *it;
             switch(cmd->getType()){
                case SVG_MOVE_TO_ABS:
                {
                 QPointF p = qtransform.map(QPointF(cmd->x,cmd->y));
                 new_path.push_back(new SVGMoveTo(p.x(),p.y(), true));

                 break;
                }
                case SVG_LINE_TO_ABS:
                {
                  QPointF p = qtransform.map(QPointF(cmd->x,cmd->y));
                  new_path.push_back(new SVGLineTo(p.x(),p.y(), true));
                  break;
                }
                case SVG_CUBIC_CURVE_TO_ABS:
                {
                        QPointF c1=qtransform.map(QPointF(((SVGCubicCurveTo*)cmd)->x0, ((SVGCubicCurveTo*)cmd)->y0));
                        QPointF c2=qtransform.map(QPointF(((SVGCubicCurveTo*)cmd)->x1, ((SVGCubicCurveTo*)cmd)->y1));
                        QPointF p2=qtransform.map(QPointF(cmd->x, cmd->y));
                        new_path.push_back(new SVGCubicCurveTo(c1.x(),c1.y(),c2.x(),c2.y(),p2.x(),p2.y(), true));
                        break;

                }
         }//end switch
   }//end for
   return new_path;
}
