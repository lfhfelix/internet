#include <QtGui>
#include <QDialog>
#include <QtNetwork/QTcpSocket>
#include<QThread>
#include <QtCore>
#include <QCoreApplication>
//#include <QTest>

#include "lookwindow.h"
#include "ui_lookwindow.h"



LookWindow::LookWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LookWindow)
{
    ui->setupUi(this);
   this->setMaximumSize(750,550);
   this->setMinimumSize(750,550);
    pixmap = new QPixmap();
    socket = NULL;
    ui->startButton->setEnabled(true);
    ui->sotpButton->setEnabled(false);

}

LookWindow::~LookWindow()
{
    delete ui;
}

void LookWindow::on_returnButton_clicked()
{
    this->hide();
    MainWindow *p = new MainWindow;
    p->show();
    socket->close();
    socket =NULL;
    close();
}

void LookWindow::on_startButton_clicked()
{

    if(socket == NULL) {
       socket = new QTcpSocket(this);
       socket->connectToHost("192.168.2.145",28888);

        QObject::connect(socket, SIGNAL(connected()),this, SLOT(connected()));
        QObject::connect(socket,SIGNAL(disconnected()),this,SLOT(disconnect()));
        QObject::connect(socket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(error()));
        ui->startButton->setEnabled(false);
        ui->sotpButton->setEnabled(true);
    }
    else {
        ui->startButton->setEnabled(false);
        ui->sotpButton->setEnabled(true);
        timer->start();
    }
}
void LookWindow::connected()
{
    qDebug()<<"connected";
    timer = new QTimer(this);

   QObject::connect(timer, SIGNAL(timeout()),this, SLOT(updatepic()));
    timer->start(100);
}

void LookWindow::disconnect()
{
    if(timer->isActive())
    {
        timer->stop();
    }
    ui->startButton->setEnabled(true);
    ui->sotpButton->setEnabled(false);
    socket->close();
    socket = NULL;
}

void LookWindow::updatepic()
{
    int ret;
    int ret1;

    char *request = "2|";
    //
    char *request1 = "4|";

    char response[6];
    //
    char response1[20];
    char *len;

    char piclength[512];
    int piclen;

   // char picbuf[1024 * 1024];


        ret = socket->write(request,strlen(request));
        if(ret != strlen(request)) {
            qDebug()<<"send request failed";
            timer->stop();
            socket->close();
        }
        socket->flush();
        socket->waitForReadyRead(3000);
        memset(response, 0, sizeof(response));

        ret = socket->read(response,sizeof(response));
     //  qDebug()<<"reti"<<response;
        if(ret != sizeof(response)) {
            qDebug()<<"recv response failed";
            timer->stop();
            socket->close();
            socket = NULL;
        }

       len = strstr(response,"|");
     //  qDebug()<<"len"<<len;

        if(len == NULL) {
            qDebug()<<"response header is error";
            timer->stop();
            socket->close();
            socket = NULL;
        }

        *len = '\0';
        //piclen= snprintf(response,);

//        sscanf(response,"%s[|]", piclength);

        piclen = atoi(response);
//        char picbuf[piclen];
        char picbuf[128 * 1024];
       qDebug()<<"piclen"<<piclen;
        int total = 0;
        while(total < piclen) {
             ret = socket->read(picbuf+total, piclen-total );
             //qDebug()<<"piclen-total"<<piclen -total;
            //socket->readAll();
           //  qDebug()<<"ret"<<ret;

            if(ret < 0) {
                qDebug()<<"recv pic failed"<< ret;
                timer->stop();
                socket->close();
                socket = NULL;
                return;
            }
            else if (ret == 0) {
                socket->waitForReadyRead(3000);
               continue;
            }
            else
            {
                total += ret;
            }
            qDebug()<<"total:"<<total;

           // qDebug()<<"picbuf"<<atoi(picbuf);
         }

            pixmap->loadFromData((const uchar *) picbuf, piclen,"JPEG");
            ui->label_2 ->setPixmap(*pixmap);
          //  sleep(2);

         ret1 = socket->write(request1,strlen(request1));

         qDebug()<<"requst1"<<request1;
         qDebug()<<"requst1"<<ret1;
         qDebug()<<"requst1"<<strlen(request1);

         if(ret1 != strlen(request1))
         {
             qDebug()<<"send request failed!";
             timer->stop();
             socket->close();
         }
         socket->flush();
         socket->waitForReadyRead(300);
         memset(response1, 0, sizeof(response1));

         ret1 = socket->read(response1,sizeof(response1));
         qDebug()<<response1;
         qDebug()<<ret1;
         qDebug()<<sizeof(response1);

         if(ret1 != sizeof(response1))
         {
             qDebug()<<"recv response1 failed!";
             timer -> stop();
             socket ->close();
             socket = NULL;
         }
         QStringList str = QString(QLatin1String(response1)).split("|");//response1.split("|");
         qDebug()<<"str" <<str;
         for ( int i = 0;i < str.count(); i++)
         {
             QString strlist = str.at(i);
             qDebug()<<"strlist"<<strlist;
         }
         QString wendu = str.at(1);
         QString shidu = str.at(2);
         QString guangzhao = str.at(3);


         QString led1 = str.at(4);

         QString fan1 = str.at(5);
         QString bee = str.at(6);
         char a[10];
         char b[10];
         char c[10];
         memset(a, 0, sizeof(a));
         memset(b, 0, sizeof(b));
         memset(c, 0, sizeof(c));
         strcpy(a,led1.toLatin1().data());
         strcpy(b, fan1.toLatin1().data());
         strcpy(c, bee.toLatin1().data());
         if(strcmp(a,"0") == 0)
         {
             strcpy(a,"close");
             ui->led->setText(a);
         }
         else {
             strcpy(a,"open");
             ui->led->setText(a);
         }
         if(strcmp(b,"0") == 0)
         {
             strcpy(b,"close");
             ui->fan->setText(b);
         }
         else {
             strcpy(b,"open");
             ui->fan->setText(b);
         }
         if(strcmp(c,"0") == 0)
         {
             strcpy(c,"close");
             ui->beep->setText(c);
         }
         else {
             strcpy(c,"open");
             ui->beep->setText(c);
         }

         ui->tempBrowser ->setText(wendu);
         ui->humBrowser->setText(shidu);
         ui->luxBrowser->setText(guangzhao);



}

void LookWindow::on_sotpButton_clicked()
{
    if(timer->isActive())
        timer->stop();
    ui->startButton->setEnabled(true);
    ui->sotpButton->setEnabled(false);
}



void LookWindow::on_tempBrowser_2_destroyed()
{

}

void LookWindow::on_openLedButton_clicked()
{
    int ret2;
    char *request2 = "5|LED_ON";
    ret2 = socket->write(request2, strlen(request2));
    qDebug()<<"request2dsfasdfadsgadfghdfhdfhqhe"<< ret2;
}

void LookWindow::on_closeLedButton_clicked()
{
    int ret3;
    char *request3 ="5|LED_OFF";
    ret3 = socket->write(request3, strlen(request3));
    qDebug()<<"djkhgdfjkhgiojdfjdh"<<ret3;

}

void LookWindow::on_openFsButton_clicked()
{
    int ret4;
    char *request4 = "6|FAN_ON";
    ret4 = socket->write(request4, strlen(request4));
     qDebug()<<"djkhgdfjkhgiojdfjdh"<<ret4;
}

void LookWindow::on_closeFsButton_clicked()
{
    int ret5;
    char *request5 = "6|FAN_OFF";
    ret5 = socket->write(request5, strlen(request5));
     qDebug()<<"djkhgdfjkhgiojdfjdh"<<ret5;
}

void LookWindow::on_openbeeButton_clicked()
{
    int ret6;
    char *request6 = "7|BUZZ_ON";
    ret6 = socket->write(request6, strlen(request6));
     qDebug()<<"djkhgdfjkhgiojdfjdh"<<ret6;
}

void LookWindow::on_closebeeButton_clicked()
{
    int ret7;
    char *request7 = "7|BUZZ_OFF";
    ret7 = socket->write(request7, strlen(request7));
     qDebug()<<"djkhgdfjkhgiojdfjdh"<<ret7;
}
