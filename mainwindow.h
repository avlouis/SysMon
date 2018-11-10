#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QShortcut>
#include <QTimer>

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

private slots:
    void on_actionQuit_triggered();

    void on_actionFullscreen_triggered();

private:
    Ui::MainWindow *ui;

    QShortcut _fullscreenShortcut;
    QShortcut _quitShortcut;

    QTimer _timer;
};

#endif // MAINWINDOW_H
