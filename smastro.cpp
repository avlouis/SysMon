#include "smastro.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUrlQuery>
#include <sstream>

// https://aa.usno.navy.mil/data/docs/api.php

SMAstro::SMAstro(QWidget *parent) : QWidget(parent),
    _mainLayout(this)
{
    // Prepare list of expected SSL errors for api.usno.navy.mil certificate
    QSslCertificate usnoCertificate =
            QSslCertificate::fromPath(":/cert/apiusnonavymil.crt").at(0);
    _usnoExpectedSSLErrors.append(
                QSslError(QSslError::UnableToGetLocalIssuerCertificate,
                          usnoCertificate));
    _usnoExpectedSSLErrors.append(
                QSslError(QSslError::UnableToVerifyFirstCertificate,
                          usnoCertificate));

    _mainLayout.addWidget(&_moon, 0, 0);
    _mainLayout.addLayout(&_dayInfoLayout, 0, 1);
    _mainLayout.addWidget(&_sunset, 1, 0);
    _mainLayout.addWidget(&_sunrise, 1, 1);

    _moon.setAlignment(Qt::AlignCenter);
    _sunrise.setAlignment(Qt::AlignCenter);
    _sunset.setAlignment(Qt::AlignCenter);

    _dayInfoLayout.addWidget(&_dayInfoToday, 0, 1);
    _dayInfoToday.setAlignment(Qt::AlignCenter);
    _dayInfoLayout.addWidget(&_dayInfoTomorrow, 0, 2);
    _dayInfoTomorrow.setAlignment(Qt::AlignCenter);
    for(int r = 0; r < DAYINFOITEM_MAX; r++) {
        for(int c = 0; c < DAYINFOITEMFIELD_MAX; c++) {
            _dayInfoLayout.addWidget(&_dayInfo[r][c], r+1, c);
            _dayInfo[r][c].setAlignment(Qt::AlignCenter);
            _dayInfo[r][c].setStyleSheet("QLabel { color: rgba(0,255,0,255); }");
        }
    }

    _dayInfoToday.setText("Today");
    _dayInfoToday.setStyleSheet("QLabel { color: rgba(0,255,0,255); }");
    _dayInfoTomorrow.setText("Tomorrow");
    _dayInfoTomorrow.setStyleSheet("QLabel { color: rgba(0,255,0,255); }");
    _dayInfo[SOLAR_BEGIN_TWILIGHT][NAME].setText("Begin Twilight");
    _dayInfo[SOLAR_RISE][NAME].setText("Sunrise");
    _dayInfo[SOLAR_UPPER_TRANSIT][NAME].setText("Upper Transit");
    _dayInfo[SOLAR_SET][NAME].setText("Sunset");
    _dayInfo[SOLAR_END_TWILIGHT][NAME].setText("End Twilight");
    _dayInfo[LUNAR_RISE][NAME].setText("Moonrise");
    _dayInfo[LUNAR_UPPER_TRANSIT][NAME].setText("Upper Transit");
    _dayInfo[LUNAR_SET][NAME].setText("Moonset");
    _dayInfo[LUNAR_PHASE][NAME].setText("Phase");
    _dayInfo[LUNAR_FRACTION][NAME].setText("Fraction");
}

void SMAstro::setNetworkAccessManager(QSharedPointer<QNetworkAccessManager> q)
{
    _qnam = q;
}

void SMAstro::resizeEvent(QResizeEvent *)
{
    displayImages();
}

void SMAstro::performImageDataRequests()
{
    QUrl sunriseReq = QUrl("https://api.usno.navy.mil/imagery/earth.png");
    QUrl sunsetReq = QUrl("https://api.usno.navy.mil/imagery/earth.png");
    QUrl moonReq = QUrl("https://api.usno.navy.mil/imagery/moon.png");

    QUrlQuery sunriseQuery(sunriseReq);
    sunriseQuery.addQueryItem("date", "today");
    sunriseQuery.addQueryItem("view", "rise");
    sunriseReq.setQuery(sunriseQuery);

    QUrlQuery sunsetQuery(sunsetReq);
    sunsetQuery.addQueryItem("date", "today");
    sunsetQuery.addQueryItem("view", "set");
    sunsetReq.setQuery(sunsetQuery);

    QUrlQuery moonQuery(moonReq);
    moonQuery.addQueryItem("date", "today");
    moonReq.setQuery(moonQuery);

    _moonReply = _qnam->get(QNetworkRequest(moonReq));
    _moonReply->ignoreSslErrors(_usnoExpectedSSLErrors);
    _sunriseReply = _qnam->get(QNetworkRequest(sunriseReq));
    _sunriseReply->ignoreSslErrors(_usnoExpectedSSLErrors);
    _sunsetReply = _qnam->get(QNetworkRequest(sunsetReq));
    _sunsetReply->ignoreSslErrors(_usnoExpectedSSLErrors);

    QObject::connect(_moonReply, SIGNAL(finished()), this, SLOT(moonReply()));
    QObject::connect(_sunriseReply, SIGNAL(finished()), this,
                     SLOT(sunriseReply()));
    QObject::connect(_sunsetReply, SIGNAL(finished()), this,
                     SLOT(sunsetReply()));
}

void SMAstro::performDayInfoDataRequests()
{
    std::stringstream ss;
    const QDateTime dt = QDateTime::currentDateTime();
    const QDate today = dt.date();
    const QDate tomorrow = today.addDays(1);
    const float tz = dt.offsetFromUtc() / 3600;
    const double lat = _geoCoordinate.latitude();
    const double lon = _geoCoordinate.longitude();

    QUrl dayInfoTodayReq = QUrl("https://api.usno.navy.mil/rstt/oneday");
    QUrl dayInfoTomorrowReq = QUrl("https://api.usno.navy.mil/rstt/oneday");

    QUrlQuery dayInfoTodayQuery(dayInfoTodayReq);
    QUrlQuery dayInfoTomorrowQuery(dayInfoTomorrowReq);

    ss.str("");
    ss << today.month() << "/" << today.day() << "/" << today.year();
    dayInfoTodayQuery.addQueryItem("date", QString::fromStdString(ss.str()));
    ss.str("");
    ss << tomorrow.month() << "/" << tomorrow.day() << "/" << tomorrow.year();
    dayInfoTomorrowQuery.addQueryItem("date", QString::fromStdString(ss.str()));

    ss.str("");
    ss << lat << "," << lon;
    dayInfoTodayQuery.addQueryItem("coords", QString::fromStdString(ss.str()));
    dayInfoTomorrowQuery.addQueryItem("coords",
                                      QString::fromStdString(ss.str()));

    dayInfoTodayQuery.addQueryItem(
                "tz", QString::fromStdString(std::to_string(tz)));
    dayInfoTomorrowQuery.addQueryItem(
                "tz", QString::fromStdString(std::to_string(tz)));

    dayInfoTodayReq.setQuery(dayInfoTodayQuery);
    dayInfoTomorrowReq.setQuery(dayInfoTomorrowQuery);

    _dayInfoTodayReply = _qnam->get(QNetworkRequest(dayInfoTodayReq));
    _dayInfoTodayReply->ignoreSslErrors(_usnoExpectedSSLErrors);

    _dayInfoTomorrowReply = _qnam->get(QNetworkRequest(dayInfoTomorrowReq));
    _dayInfoTomorrowReply->ignoreSslErrors(_usnoExpectedSSLErrors);

    QObject::connect(_dayInfoTodayReply, SIGNAL(finished()), this,
                     SLOT(dayInfoTodayReply()));
    QObject::connect(_dayInfoTomorrowReply, SIGNAL(finished()), this,
                     SLOT(dayInfoTomorrowReply()));
}

void SMAstro::geoCoordinate(const QGeoCoordinate & c)
{
    _geoCoordinate = c;
}

void SMAstro::displayImages()
{
    _moon.setPixmap(QPixmap::fromImage(
                        _moonImage.scaled(_moon.width(), _moon.height(),
                                          Qt::KeepAspectRatio,
                                          Qt::SmoothTransformation)));
    _sunrise.setPixmap(QPixmap::fromImage(
                           _sunriseImage.scaled(_sunrise.width(),
                                                _sunrise.height(),
                                                Qt::KeepAspectRatio,
                                                Qt::SmoothTransformation)));
    _sunset.setPixmap(QPixmap::fromImage(
                          _sunsetImage.scaled(_sunset.width(), _sunset.height(),
                                              Qt::KeepAspectRatio,
                                              Qt::SmoothTransformation)));
}

void SMAstro::moonReply()
{
    qDebug() << "Moon replied";
    // TODO handle errors
    _moonImage.loadFromData(_moonReply->readAll());
    _moonImage = _moonImage.copy(QRect(204,204,616,616));
    _moonReply->deleteLater();

    displayImages();
}

void SMAstro::sunriseReply()
{
    qDebug() << "Sunrise replied";
    // TODO handle errors
    _sunriseImage.loadFromData(_sunriseReply->readAll());
    _sunriseImage = _sunriseImage.copy(QRect(204,204,616,616));
    _sunriseReply->deleteLater();

    displayImages();
}

void SMAstro::sunsetReply()
{
    qDebug() << "Sunset replied";
    // TODO handle errors
    _sunsetImage.loadFromData(_sunsetReply->readAll());
    _sunsetImage = _sunsetImage.copy(QRect(204,204,616,616));
    _sunsetReply->deleteLater();

    displayImages();
}

void SMAstro::dayInfoTodayReply()
{
    qDebug() << "DayInfoToday replied";
    // TODO handle errors
    QJsonDocument doc =
            QJsonDocument::fromJson(_dayInfoTodayReply->readAll());

    parseDayInfo(doc, TODAY);

    _dayInfoTodayReply->deleteLater();
}

void SMAstro::dayInfoTomorrowReply()
{
    qDebug() << "DayInfoTomorrow replied";
    // TODO handle errors
    QJsonDocument doc =
            QJsonDocument::fromJson(_dayInfoTomorrowReply->readAll());

    parseDayInfo(doc, TOMORROW);

    _dayInfoTomorrowReply->deleteLater();
}

void SMAstro::parseDayInfo(const QJsonDocument & doc, int idx)
{
    qDebug() << doc;
    QJsonArray sundata = doc["sundata"].toArray();
    QJsonArray moondata = doc["moondata"].toArray();

    for(QJsonValue v : sundata) {
        QJsonObject o = v.toObject();
        QString phen = o["phen"].toString();
        QString time = o["time"].toString();

        if(phen == "BC") {
            _dayInfo[SOLAR_BEGIN_TWILIGHT][idx].setText(time);
        } else if(phen == "R") {
            _dayInfo[SOLAR_RISE][idx].setText(time);
        } else if(phen == "U") {
            _dayInfo[SOLAR_UPPER_TRANSIT][idx].setText(time);
        } else if(phen == "S") {
            _dayInfo[SOLAR_SET][idx].setText(time);
        } else if(phen == "EC") {
            _dayInfo[SOLAR_END_TWILIGHT][idx].setText(time);
        } else {
            qDebug() << "Unknown solar phenomenon: " << phen;
        }
    }

    for(QJsonValue v : moondata) {
        QJsonObject o = v.toObject();
        QString phen = o["phen"].toString();
        QString time = o["time"].toString();

        if(phen == "R") {
            _dayInfo[LUNAR_RISE][idx].setText(time);
        } else if(phen == "U") {
            _dayInfo[LUNAR_UPPER_TRANSIT][idx].setText(time);
        } else if(phen == "S") {
            _dayInfo[LUNAR_SET][idx].setText(time);
        } else {
            qDebug() << "Unkown lunar phenomenon: " << phen;
        }
    }

    _dayInfo[LUNAR_PHASE][idx].setText(doc["curphase"].toString());
    _dayInfo[LUNAR_FRACTION][idx].setText(doc["fracillum"].toString());
}
