#ifndef SMASTRO_H
#define SMASTRO_H

#include <QGeoCoordinate>
#include <QGridLayout>
#include <QLabel>
#include <QNetworkReply>
#include <QWidget>

class SMAstro : public QWidget
{
    Q_OBJECT
public:
    explicit SMAstro(QWidget *parent = nullptr);

    void setNetworkAccessManager(QSharedPointer<QNetworkAccessManager> q);

signals:

protected slots:
    void resizeEvent(QResizeEvent * e);

public slots:
    void performImageDataRequests();
    void performDayInfoDataRequests();
    void geoCoordinate(const QGeoCoordinate & c);

private slots:
    void displayImages();
    void moonReply();
    void sunriseReply();
    void sunsetReply();
    void dayInfoTodayReply();
    void dayInfoTomorrowReply();

private:
    QSharedPointer<QNetworkAccessManager> _qnam;
    QImage _moonImage;
    QImage _sunriseImage;
    QImage _sunsetImage;
    QNetworkReply * _moonReply;
    QNetworkReply * _sunriseReply;
    QNetworkReply * _sunsetReply;
    QNetworkReply * _dayInfoTodayReply;
    QNetworkReply * _dayInfoTomorrowReply;
    QList<QSslError> _usnoExpectedSSLErrors;

    QGridLayout _mainLayout;

    QLabel _moon;
    QLabel _sunrise;
    QLabel _sunset;

    QGeoCoordinate _geoCoordinate;

    enum DayInfoItem {
        SOLAR_BEGIN_TWILIGHT,
        SOLAR_RISE,
        SOLAR_UPPER_TRANSIT,
        SOLAR_SET,
        SOLAR_END_TWILIGHT,
        LUNAR_RISE,
        LUNAR_UPPER_TRANSIT,
        LUNAR_SET,
        LUNAR_PHASE,
        LUNAR_FRACTION,
        DAYINFOITEM_MAX
    };
    enum DayInfoItemField {
        NAME,
        TODAY,
        TOMORROW,
        DAYINFOITEMFIELD_MAX
    };
    QGridLayout _dayInfoLayout;
    QLabel _dayInfoToday;
    QLabel _dayInfoTomorrow;
    QLabel _dayInfo[DAYINFOITEM_MAX][DAYINFOITEMFIELD_MAX];

    void parseDayInfo(const QJsonDocument & doc, int idx);
};

#endif // SMASTRO_H
