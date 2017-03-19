#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QMessageBox>
#include<QLineEdit>




#include<QTimer>
#include<QtNetwork/QTcpSocket>

#include"lookwindow.h"
#include"specificationwindow.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_submitButton_clicked();

    void on_cancelButto_clicked();

    void connected();
    void disconnect();
    void error(QAbstractSocket::SocketError);
    void update();

    void on_specificationButton_clicked();

private:
    Ui::MainWindow *ui;
    QTcpSocket *socket;
       QTimer *timer;


};

#endif // MAINWINDOW_H
