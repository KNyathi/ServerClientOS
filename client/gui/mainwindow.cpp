#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      socket1(new QTcpSocket(this)),
      socket2(new QTcpSocket(this))
{
    ui->setupUi(this);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::updateStatusLabel() {
    bool isConnected1 = socket1->state() == QAbstractSocket::ConnectedState;
    bool isConnected2 = socket2->state() == QAbstractSocket::ConnectedState;

    if (isConnected1 && isConnected2) {
        ui->label->setText("Connected to Server 1 and Server 2");
    } else if (isConnected1) {
        ui->label->setText("Connected to Server 1");
    } else if (isConnected2) {
        ui->label->setText("Connected to Server 2");
    } else {
        ui->label->setText("Not connected to any server");
    }
}

// Connect to Server 1
void MainWindow::on_pushButton_clicked() {
    socket1->connectToHost("127.0.0.1", 5001);
    if (socket1->waitForConnected(3000)) {
        ui->textEdit->setText("Connected to Server 1.");
    } else {
        ui->textEdit->setText("Failed to connect to Server 1.");
    }
    updateStatusLabel();
}

// Disconnect from Server 1
void MainWindow::on_pushButton_2_clicked() {
    socket1->disconnectFromHost();
    if (socket1->state() != QAbstractSocket::UnconnectedState) {
        socket1->waitForDisconnected(1000);
    }
    ui->textEdit->setText("Disconnected from Server 1.");
    updateStatusLabel();
}

// Connect to Server 2
void MainWindow::on_pushButton_3_clicked() {
    socket2->connectToHost("127.0.0.1", 5002);
    if (socket2->waitForConnected(3000)) {
        ui->textEdit_2->setText("Connected to Server 2.");
    } else {
        ui->textEdit_2->setText("Failed to connect to Server 2.");
    }
    updateStatusLabel();
}

// Disconnect from Server 2
void MainWindow::on_pushButton_4_clicked() {
    socket2->disconnectFromHost();
    if (socket2->state() != QAbstractSocket::UnconnectedState) {
        socket2->waitForDisconnected(1000);
    }
    ui->textEdit_2->setText("Disconnected from Server 2.");
    updateStatusLabel();
}

// Request Info from Server 1
void MainWindow::on_pushButton_5_clicked() {
    if (socket1->state() == QAbstractSocket::ConnectedState) {
        socket1->write("INFO");
        if (socket1->waitForReadyRead(3000)) {
            QString response = socket1->readAll();
            ui->textEdit->setText(response);
        } else {
            ui->textEdit->setText("No response from Server 1.");
        }
    } else {
        ui->textEdit->setText("Server 1 not connected.");
    }
}

// Request Info from Server 2
void MainWindow::on_pushButton_6_clicked() {
    if (socket2->state() == QAbstractSocket::ConnectedState) {
        socket2->write("INFO");
        if (socket2->waitForReadyRead(3000)) {
            QString response = socket2->readAll();
            ui->textEdit_2->setText(response);
        } else {
            ui->textEdit_2->setText("No response from Server 2.");
        }
    } else {
        ui->textEdit_2->setText("Server 2 not connected.");
    }
}