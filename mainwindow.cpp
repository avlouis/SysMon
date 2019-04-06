#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDateTime>
#include <QDebug>
#include <QPainter>
#include <QProcess>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QTemporaryFile>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _fullscreenShortcut(this),
    _quitShortcut(this)
{
    ui->setupUi(this);

    _qnam = QSharedPointer<QNetworkAccessManager>(new QNetworkAccessManager(this));

    _fullscreenShortcut.setKey(QKeySequence("F11"));
    _fullscreenShortcut.setEnabled(false);
    connect(&_fullscreenShortcut, SIGNAL(activated()), this, SLOT(on_actionFullscreen_triggered()));
    _quitShortcut.setKey(QKeySequence("Ctrl+Q"));
    _quitShortcut.setEnabled(false);
    connect(&_quitShortcut, SIGNAL(activated()), this, SLOT(on_actionQuit_triggered()));

    ui->ramDonut->setHolePercent(100.0/150.0);
    ui->ramDonut->setRingCount(2);
    ui->ramDonut->setRingColor(0, Qt::darkGreen);
    ui->ramDonut->setRingColor(1, Qt::green);
    ui->clock->setColor(Qt::green);
    ui->astro->setNetworkAccessManager(_qnam);

    _geoSource = QGeoPositionInfoSource::createDefaultSource(this);
    if(_geoSource) {
        qDebug() << _geoSource->sourceName();
        qDebug() << _geoSource->error();
        connect(_geoSource, SIGNAL(positionUpdated(QGeoPositionInfo)),
                this, SLOT(positionUpdated(QGeoPositionInfo)));
        _geoSource->startUpdates();
    }

    _timer10Hz.setInterval(100);
    _timer10Hz.setSingleShot(false);
    connect(&_timer10Hz, SIGNAL(timeout()), this, SLOT(test()));
    _timer10Hz.start();

    ui->astro->performImageDataRequests();
    _timer_5min.setInterval(5*60*1000);
    _timer_5min.setSingleShot(false);
    connect(&_timer_5min, SIGNAL(timeout()), ui->astro, SLOT(performImageDataRequests()));
    _timer_5min.start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *)
{
    // Do manual layout
    QSize s = this->centralWidget()->size();
    int w = s.width() / 2;
    int h = s.height() / 2;

    ui->ramDonut->move(0,0);
    ui->ramDonut->resize(w,h);

    ui->clock->move(w,0);
    ui->clock->resize(w,h);

    ui->astro->move(0,h);
    ui->astro->resize(2*w,h);
}

void MainWindow::positionUpdated(const QGeoPositionInfo & info)
{
    _geoSource->stopUpdates();
    qDebug() << "Position updated: " << info;
    ui->astro->geoCoordinate(info.coordinate());
    ui->astro->performDayInfoDataRequests();
}

void MainWindow::test()
{
    static int i = 0;
    if(i == 0) {
        ui->clock->update();
    }
    i = (i+1) % 10;

    QProcess proc;
    proc.start("awk",
               QStringList() << "/MemTotal/ { print $2 }" << "/proc/meminfo");
    proc.waitForFinished();
    QString memTotalKB = proc.readAllStandardOutput();
    proc.close();
    proc.start("awk",
               QStringList() << "/MemFree/ { print $2 }" << "/proc/meminfo");
    proc.waitForFinished();
    QString memFreeKB = proc.readAllStandardOutput();
    proc.close();
    proc.start("awk",
               QStringList() << "/MemAvailable/ { print $2 }" << "/proc/meminfo");
    proc.waitForFinished();
    QString memAvailKB = proc.readAllStandardOutput();
    proc.close();
    ui->ramDonut->setRingPercent(0, 1.0 -
                                 (double)memFreeKB.toLong() /
                                 (double)memTotalKB.toLong());
    ui->ramDonut->setRingPercent(1, 1.0 -
                                 (double)memAvailKB.toLong() /
                                 (double)memTotalKB.toLong());

    proc.start("uptime",
               QStringList() << "-s");
    proc.waitForFinished();
    QString systemStart = proc.readAllStandardOutput();
    proc.close();
    QDateTime systemBoot = QDateTime::fromString(systemStart.trimmed(),
                                                 "yyyy-MM-dd HH:mm:ss");
    qint64 bootSecs = systemBoot.secsTo(QDateTime::currentDateTime());
    qint64 bootDays = bootSecs / (60 * 60 * 24);
    bootSecs = bootSecs % (60 * 60 * 24);
    qint64 bootHours = bootSecs / (60 * 60);
    bootSecs = bootSecs % (60 * 60);
    qint64 bootMinutes = bootSecs / (60);
    bootSecs = bootSecs % (60);

    //qDebug() << bootDays << "d " << bootHours << "h " << bootMinutes << "m " << bootSecs << "s";

    // Installed package count
    // pacman -Qq | wc -l

    // Updates waiting
    // pacman -Qu | wc -l

    // Disk partition usage
    // parse df

    // System creation date
    // head -1 /var/log/pacman.log | cut -d\  -f1,2


    // Missing
    // -------
    // geoclue for location
    // memory
    //     - Description text
    //     - Swap
    //     - Worst offenders
    // network
    //     - Address(es)
    //     - Throughput
    //     - Worst offenders (processes, dests/sources)
    // battery/power
    // cpu
    //     - Description text
    //     - Per core usage
    //     - Per core frequency
    //     - Worst offenders
    // temperature(s)
    // system audio spectrum analyzer
    // recent errors
    //     - failed systemd units
    //     - dmesg errors
    // audio track info
    //     - album art
    //     - name, album, artist
    //     - track progress
    // weather
    //     - https://www.weather.gov/documentation/services-web-api
    //     - alerts
    //     - radar
    //     - temperature, pressure, wind, etc
    //     - sunrise, sunset, phase of moon, etc
}

void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    // Set background
    painter.fillRect(this->centralWidget()->rect(), Qt::black);
}

void MainWindow::on_actionFullscreen_triggered()
{
    if(this->isFullScreen()) {
        this->showNormal();
        this->menuBar()->show();
        _fullscreenShortcut.setEnabled(false);
        _quitShortcut.setEnabled(false);
    } else {
        this->showFullScreen();
        this->menuBar()->hide();
        _fullscreenShortcut.setEnabled(true);
        _quitShortcut.setEnabled(true);
    }
}

void MainWindow::on_actionQuit_triggered()
{
    QApplication::quit();
}
