#include "smdonutplot.h"

#include <QPainter>

#include <math.h>

SMDonutPlot::SMDonutPlot(QWidget *parent) : QWidget(parent)
{
    _ringPercent = QVector<qreal>({0.0});
    _holePercent = 0.0;
}

void SMDonutPlot::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    qreal orad = std::min(this->width(), this->height()) / 2.0;
    qreal irad = orad * _holePercent;

    // construct painter path
    QPointF center = QPointF(this->width()/2.0, this->height()/2.0);
    QRectF obb = QRectF(center - QPointF(orad, orad), QSizeF(2*orad, 2*orad));
    QRectF ibb = QRectF(center - QPointF(irad, irad), QSizeF(2*irad, 2*irad));
    QPointF ostart = center - QPointF(0,orad);

    for(int i = 0; i < _ringPercent.size(); i++) {
        qreal rp = _ringPercent[i];
        QPointF iend = center + QPointF(irad*cos(M_PI/2 - rp*2*M_PI),
                                        -irad*sin(M_PI/2 - rp*2*M_PI));
        qreal ringDegrees = 360*rp;
        QPainterPath path;
        path.moveTo(ostart);
        // TODO make start orientation configurable
        path.arcTo(obb, 90, -ringDegrees);
        path.lineTo(iend);
        path.arcTo(ibb, 90 - ringDegrees, ringDegrees);
        path.closeSubpath();

        painter.fillPath(path, _ringColor[i]); // TODO make this configurable
    }
}

qreal SMDonutPlot::holePercent() const
{
    return _holePercent;
}

void SMDonutPlot::setHolePercent(const qreal &holePercent)
{
    if(holePercent < 0.0) {
        _holePercent = 0.0;
    } else if(holePercent > 1.0) {
        _holePercent = 1.0;
    } else {
        _holePercent = holePercent;
    }
    this->update();
}

int SMDonutPlot::ringCount() const
{
    return _ringPercent.length();
}

void SMDonutPlot::setRingCount(int i)
{
    _ringPercent.resize(i);
    _ringColor.resize(i);
}

QColor SMDonutPlot::ringColor(int i) const
{
    return _ringColor[i];
}

void SMDonutPlot::setRingColor(int i, const QColor &c)
{
    _ringColor[i] = c;
}

qreal SMDonutPlot::ringPercent(int i) const
{
    return _ringPercent[i];
}

void SMDonutPlot::setRingPercent(int i, const qreal &ringPercent)
{
    if(ringPercent < 0.0) {
        _ringPercent[i] = 0.0;
    } else if(ringPercent > 1.0) {
        _ringPercent[i] = 1.0;
    } else {
        _ringPercent[i] = ringPercent;
    }
    this->update();
}
