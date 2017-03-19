#ifndef LOOKWINDOW_H
#define LOOKWINDOW_H


//#include <QTest>
#include <QWidget>
#include <QThread>
#include<QTimer>
#include<QtNetwork/QTcpSocket>


#include"messagewindow.h"
#include"mainwindow.h"




namespace Ui {
class LookWindow;
}

class LookWindow : public QWidget
{
    Q_OBJECT

public:
    explicit LookWindow(QWidget *parent = 0);
    ~LookWindow();

private slots:
    void on_returnButton_clicked();
    void connected();
    void disconnect();
    void updatepic();



    void on_startButton_clicked();

    void on_sotpButton_clicked();



    void on_tempBrowser_2_destroyed();

    void on_openLedButton_clicked();

    void on_closeLedButton_clicked();

    void on_openFsButton_clicked();

    void on_closeFsButton_clicked();

    void on_openbeeButton_clicked();

    void on_closebeeButton_clicked();

private:
    Ui::LookWindow *ui;
    QTcpSocket *socket;
     QTimer *timer;
     QPixmap *pixmap;

};

#endif // LOOKWINDOW_H
