#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    dlnaf = new DLNAFinder();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete dlnaf;
}

void MainWindow::on_pushButton_clicked()
{
    dlnaf->find();
}

void MainWindow::on_pushButton_2_clicked()
{
    qDebug();
}
