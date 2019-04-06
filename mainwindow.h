#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QShortcut>
#include <QTimer>
#include <QNetworkReply>
#include <QGeoPositionInfo>
#include <QGeoPositionInfoSource>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent * event);
    void resizeEvent(QResizeEvent * event);

private slots:
    void on_actionFullscreen_triggered();
    void on_actionQuit_triggered();
    void positionUpdated(const QGeoPositionInfo & info);

    void test();

private:
    Ui::MainWindow *ui;

    QGeoPositionInfoSource * _geoSource;

    QSharedPointer<QNetworkAccessManager> _qnam;

    QShortcut _fullscreenShortcut;
    QShortcut _quitShortcut;

    QTimer _timer10Hz;
    QTimer _timer_5min;
};

#endif // MAINWINDOW_H
