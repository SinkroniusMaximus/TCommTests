#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    Serial.begin("COM3", 115200);
    serialSub.Xinit(&Serial);
    dataSource.cmdMotorFast = 0;
    dataSource.cmdMotorSpeed = 0;
    dataSource.cmdRunMotor = false;
    dataSource.stsMotorSpeed = 0;
    dataSource.stsMotorRun = 0;
    connect(&timer, SIGNAL (timeout()), this, SLOT (timedLoop()));
    connect(ui->cmdMotorRun, SIGNAL(toggled(bool)), this, SLOT(cmdMotorRunChanged(bool)));
    connect(ui->cmdMotorSpeed, SIGNAL(valueChanged(int)), this, SLOT(cmdMotorSpeedChanged(int)));
    timer.start(1);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::timedLoop()
{
    Communicator.Xchange();
    //Status
    ui->stsMotorSpeed->display(dataSource.stsMotorSpeed);
    if(!dataSource.stsMotorRun)
    {
        ui->stsMotorRun->setText("Stopped");
    }
    else
    {
        ui->stsMotorRun->setText("Running");
    }
    timer.start(1);
}

void MainWindow::cmdMotorRunChanged(bool checked)
{
    dataSource.cmdRunMotor = checked;
}
void MainWindow::cmdMotorSpeedChanged(int newSpeed)
{
    dataSource.cmdMotorSpeed = (float)newSpeed;
}
