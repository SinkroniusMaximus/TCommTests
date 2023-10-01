#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "Data.h"
#include "../Lib/WinSerial.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        MainWindow(QWidget *parent = nullptr);
        ~MainWindow();

    private:
        Ui::MainWindow *ui;
        Data dataSource;
        QTimer timer;
        HardwareSerial Serial;
        SerialSubscriber serialSub;

    private slots:
        void timedLoop();
        void cmdMotorRunChanged(bool checked);
        void cmdMotorSpeedChanged(int newSpeed);
};
#endif // MAINWINDOW_H
