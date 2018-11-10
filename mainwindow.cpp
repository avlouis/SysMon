#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPainter>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _fullscreenShortcut(this),
    _quitShortcut(this)
{
    ui->setupUi(this);

    _fullscreenShortcut.setKey(QKeySequence("F11"));
    _fullscreenShortcut.setEnabled(false);
    connect(&_fullscreenShortcut, SIGNAL(activated()), this, SLOT(on_actionFullscreen_triggered()));
    _quitShortcut.setKey(QKeySequence("Ctrl+Q"));
    _quitShortcut.setEnabled(false);
    connect(&_quitShortcut, SIGNAL(activated()), this, SLOT(on_actionQuit_triggered()));

    _timer.setInterval(500);
    _timer.setSingleShot(false);
    connect(&_timer, SIGNAL(timeout()), this, SLOT(repaint()));
    _timer.start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent * event)
{
    QPainter painter(this);

    // Set background
    painter.fillRect(this->centralWidget()->rect(), Qt::black);

    painter.setPen(Qt::green);

    // Draw Time
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QFont font = painter.font();
    painter.setFont(QFont("monospace", 32, 2));
    painter.drawText(0, this->size().height()/2 + 35, currentDateTime.time().toString("H:mm:ss"));
    painter.setFont(font);
    painter.drawText(0, this->size().height()/2, currentDateTime.date().toString("dddd d MMMM yyyy"));
}

void MainWindow::on_actionQuit_triggered()
{
    QApplication::quit();
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
