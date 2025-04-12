#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();      // Connect to Server 1
    void on_pushButton_2_clicked();    // Disconnect from Server 1
    void on_pushButton_3_clicked();    // Connect to Server 2
    void on_pushButton_4_clicked();    // Disconnect from Server 2
    void on_pushButton_5_clicked();    // Request Info from Server 1
    void on_pushButton_6_clicked();    // Request Info from Server 2

private:
    void updateStatusLabel();          // Helper to refresh status label

    Ui::MainWindow *ui;
    QTcpSocket *socket1;
    QTcpSocket *socket2;
};

#endif // MAINWINDOW_H