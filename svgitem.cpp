#include "svgitem.h"
#include <QFile>
#include <QDebug>
#include <QXmlStreamReader>
#include <QProcess>
#include <QGraphicsScene>
#include <QCoreApplication>
#include "util.h"
SvgItem::SvgItem(const QString &fileName, QGraphicsItem *parentItem):
    QGraphicsSvgItem (fileName,parentItem)
{
     setWorkSpace(QRect(0,0,320,220));
     setOriginOffset(QPointF(-160,0));
     m_coordinate_scale = 2;
     initD(fileName);
}
SvgItem::~SvgItem()
{
    m_d->clear();
    delete m_d;
}
bool SvgItem::initD(const QString &fileName)
{
    QFile fin(fileName);
    if(!fin.open(QFile::ReadOnly | QFile::Text)) {
         qDebug() << "failed to open 2_1128.xml";
         return false;
    }
    QXmlStreamReader xml(&fin);
    while (!xml.atEnd()) {
//        qDebug() <<"xml name:" << xml.name();
        if(xml.name()=="path"&&xml.isStartElement()==true){
//            qDebug()<<"this is path";
//            qDebug()<<xml.attributes().hasAttribute("d");
            QStringRef dRef = xml.attributes().value("d");
//            qDebug()<<dRef.size();
//            qDebug()<<dRef.toUtf8();
            if(setD(dRef.toUtf8()))
                break;

        }
        xml.readNext();
    }
    if (xml.hasError()) {
    }
    fin.close();
    return true;
}

QByteArray * SvgItem::getD()
{
    return m_d;
}

bool SvgItem::setD(QByteArray d)
{
    if(d.size()==0)
        return false;
    m_d = new QByteArray(d);
    return true;
}
/*you need to reset the elements position of  QTransform
*
* in Qt
*      m11 m12 m13
*      m21 m22 m23
*      m31 m32 m33
* in gogcode
*      m11 m21 m31
*      m12 m22 m32
*      m13 m23 m33
*/

QString SvgItem::transformToStr()
{
    QString str;
    qreal scaleFactor = this->scale();
    qreal angle = this->rotation();
    QPointF pos=this->pos();

    QTransform tr= this->transform();
    QTransform posTrans;
    posTrans.translate(pos.x(),pos.y());
    posTrans.scale(scaleFactor,scaleFactor);
    posTrans.rotate(angle);
    tr*=posTrans;
    //change the coordinate
    /*example:
     *    factor = 2;
     *    workrect = QRect(0,0,320,220)
    */
    QTransform coorTrans;
    qreal factor  = getCoordinateScale();
    QRect workrect =getWorkSpace();
    coorTrans.scale(-1,1);
    coorTrans.rotate(-90);
    coorTrans.translate(-workrect.width()/2,-workrect.height());
    coorTrans.scale(1/factor,1/factor);
    tr*=coorTrans;

    str=QString("%1_%2_%3_%4_%5_%6_%7_%8_%9").arg(tr.m11()).arg(tr.m12()).arg(tr.m13())
                                             .arg(tr.m21()).arg(tr.m22()).arg(tr.m23())
                                             .arg(tr.m31()).arg(tr.m32()).arg(tr.m33());
    return str;
}
bool SvgItem::exportGcode(const QString &fileName )
{
    qDebug()<<"export Gcode";
    if(fileName.isEmpty())
        return false;
    QString d_path = replaceSuffix(fileName,"txt");
    QFile fout(d_path);
     if(!fout.open(QFile::WriteOnly | QFile::Text)) {
        qDebug() << "failed to save d";
        return false;
   }
   if(m_d->isEmpty()){
         fout.close();
         return false;
   }
   fout.write(*m_d);
   fout.close();
   QProcess p(0);
   QString currentPath = QCoreApplication::applicationDirPath();

//   QString gogcode = currentPath+"/gogcode.exe";
   QString gogcode = "F:/start/QT/gogcode/dist/gogcode.exe";
   QStringList arg;
   arg<<"inputd"<<"--file"<<d_path<<"--output"<<replaceSuffix(fileName,"gcode")<<"--transform"<<transformToStr();
   qDebug()<<arg;
   p.start(gogcode,arg);
   p.waitForStarted();
   p.waitForFinished();
//   qDebug()<<QString::fromLocal8Bit(p.readAllStandardOutput());
   return true;
}
bool SvgItem::setWorkSpace(QRect rect)
{
    m_workSpace = rect;
    return true;
}

QRect SvgItem::getWorkSpace()
{
    return m_workSpace;
}

bool SvgItem::setOriginOffset(qreal dx ,qreal dy)
{
    return setOriginOffset(QPointF(dx,dy));
}

bool SvgItem::setOriginOffset(QPointF offset){
    m_origin_offset = offset;
    return true;
}

QPointF SvgItem::getOriginOffset()
{
    return m_origin_offset;
}
qreal SvgItem::getCoordinateScale()
{
    return m_coordinate_scale;
}


void SvgItem::setCoordinateScale(qreal factor)
{
    if(factor!=0)
        m_coordinate_scale = factor;
}
