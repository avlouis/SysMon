#ifndef SMSCALABLELABEL_H
#define SMSCALABLELABEL_H

#include <QLabel>
#include <QResizeEvent>

class SMScalableLabel : public QLabel
{
public:
    explicit SMScalableLabel(QWidget * parent = nullptr);

public slots:
    void setColor(const QColor & c);
    void setTextAndScale(const QString & t);

protected:
    void resizeEvent(QResizeEvent * event);

private:
    void scale();
};

// Notes:
// Keep track of whether we are horizontally or vertically bound
// Keep track of current font size
// These will provide hints on which way font size needs to go
//
// Set font size on text change, widget resize, and font change

#endif // SMSCALABLELABEL_H
