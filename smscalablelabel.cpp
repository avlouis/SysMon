#include "smscalablelabel.h"

#include <QDebug>
#include <QFontMetrics>

SMScalableLabel::SMScalableLabel(QWidget * parent) : QLabel(parent)
{

}

void SMScalableLabel::setColor(const QColor & c)
{
    QString sheet;
    QTextStream(&sheet) << "QLabel { color: rgba("
                        << c.red() << ","
                        << c.green() << ","
                        << c.blue() << ","
                        << c.alpha() << "); }";
    this->setStyleSheet(sheet);
}

void SMScalableLabel::setTextAndScale(const QString & t)
{
    QLabel::setText(t);
    this->scale();
}

void SMScalableLabel::resizeEvent(QResizeEvent *)
{
    this->scale();
}

void SMScalableLabel::scale()
{
    // TODO this should take hints as a parameter(s)

    QSize currentSize = this->size();

//    qDebug() << "Scaling text...";

    QFont f = this->font();
    QFontMetrics metrics(f);
    QRect bb = metrics.boundingRect(this->text());

//    qDebug() << "Current font size: " << f.pointSize();
//    qDebug() << "Current bounding box: " << bb;
//    qDebug() << "Current size: " << currentSize;

    if(bb.width() + bb.x() > currentSize.width() ||
            bb.height() + bb.y() > currentSize.height()) {
        // Need to reduce font
        do {
            f.setPointSize(f.pointSize()-1);
            metrics = QFontMetrics(f);
            bb = metrics.boundingRect(this->text());
        } while(bb.width() + bb.x() > currentSize.width() ||
                bb.height() + bb.y() > currentSize.height());
    } else if(bb.width() + bb.x() < currentSize.width() &&
              bb.height() + bb.y() < currentSize.height()) {
        // Can increase font
        do {
            f.setPointSize(f.pointSize()+1);
            metrics = QFontMetrics(f);
            bb = metrics.boundingRect(this->text());
        } while(bb.width() + bb.x() < currentSize.width() &&
                bb.height() + bb.y() < currentSize.height());
        f.setPointSize(f.pointSize()-1);
    }
    this->setFont(f);

//    qDebug() << "New font size: " << f.pointSize();
//    metrics = QFontMetrics(f);
//    qDebug() << "New bounding box: " << metrics.boundingRect(this->text());
}
