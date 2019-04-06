#ifndef SMDONUTPLOT_H
#define SMDONUTPLOT_H

#include <QWidget>

class SMDonutPlot : public QWidget
{
    Q_OBJECT
public:
    explicit SMDonutPlot(QWidget *parent = nullptr);

    qreal holePercent() const;
    void setHolePercent(const qreal &holePercent);
    int ringCount() const;
    void setRingCount(int i);
    QColor ringColor(int i) const;
    void setRingColor(int i, const QColor &c);
    qreal ringPercent(int i) const;
    void setRingPercent(int i, const qreal &ringPercent);

signals:

public slots:

protected:
    void paintEvent(QPaintEvent * event);

private:
    qreal _holePercent;
    QVector<qreal> _ringPercent;
    QVector<QColor> _ringColor;
};

#endif // SMDONUTPLOT_H
