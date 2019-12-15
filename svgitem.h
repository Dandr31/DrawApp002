#ifndef SVGITEM_H
#define SVGITEM_H
#include <QGraphicsSvgItem>

class SvgItem:public QGraphicsSvgItem
{
public:
    SvgItem(const QString &fileName, QGraphicsItem *parentItem = nullptr);

    ~SvgItem() override;

    bool initD(const QString &fileName);

    bool setD(QByteArray d);

    QByteArray * getD();

    bool setWorkSpace(QRect rect);

    QRect getWorkSpace();

    bool setOriginOffset(qreal dx ,qreal dy);

    bool setOriginOffset(QPointF offset);

    QPointF getOriginOffset();

    qreal getCoordinateScale();

    void setCoordinateScale(qreal factor);

    bool exportGcode(const QString &fileName);

    QString transformToStr();

    QTransform getFinalTransfrom();
    enum { Type = UserType + 3 };
    int type() const override
      {
               // Enable the use of qgraphicsitem_cast with this item.
               return Type;
     }
private:
    QByteArray *m_d;
    QRect  m_workSpace;
    QPointF m_origin_offset;
    qreal m_coordinate_scale;
};

#endif // SVGITEM_H
