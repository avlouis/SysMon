#ifndef SMCLOCK_H
#define SMCLOCK_H

#include <QVBoxLayout>
#include <QWidget>

#include "smscalablelabel.h"

class SMClock : public QWidget
{
    Q_OBJECT
public:
    explicit SMClock(QWidget *parent = nullptr);

    void setColor(const QColor & c);

signals:

public slots:
    void update();

private:
    SMScalableLabel _dateLabel;
    SMScalableLabel _timeLabel;
    QVBoxLayout _layout;
};

#endif // SMCLOCK_H
