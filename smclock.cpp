#include "smclock.h"

#include <QDateTime>
#include <QPainter>

SMClock::SMClock(QWidget *parent) : QWidget(parent),
    _layout(this)
{
    _dateLabel.setText("filler");
    _dateLabel.setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    _timeLabel.setText("filler");
    _timeLabel.setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    _layout.addWidget(&_dateLabel);
    _layout.addWidget(&_timeLabel);
}

void SMClock::setColor(const QColor & c)
{
    _dateLabel.setColor(c);
    _timeLabel.setColor(c);
}

void SMClock::update()
{
    QDateTime currentDateTime = QDateTime::currentDateTime();
    _dateLabel.setTextAndScale(currentDateTime.date().toString("ddd dd MMM yyyy"));
    _timeLabel.setTextAndScale(currentDateTime.time().toString("HH:mm:ss"));
}
