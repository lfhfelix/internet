#include "mainwindow.h"

#include "ui_mainwindow.h"
#include <QtGui>
#include <QDialog>
#include <QtNetwork/QTcpSocket>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setMaximumSize(300,230);
    this->setMinimumSize(300,230);
    ui->passwordLineEdit->setEchoMode(QLineEdit::Password);

    socket = NULL;
    ui->submitButton->setEnabled(true);
    ui->cancelButto->setEnabled(false);
    

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_submitButton_clicked()

{
       // timer->start(300);
        if(socket == NULL)
        {
            socket = new QTcpSocket(this);
            socket->connectToHost("192.168.2.145",28888);
           // socket->waitForConnected();
            connect(socket, SIGNAL(connected()), this, SLOT(connected()));
            connect(socket,SIGNAL(disconnected()),this, SLOT(disconnect()));
            qDebug()<<"9";
            connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error()));
            qDebug()<<"10";
            ui->submitButton->setEnabled(false);
            ui->cancelButto->setEnabled(true);

        }
        else
        {
            ui->submitButton->setEnabled(false);
            ui->cancelButto->setEnabled(true);
            timer->start(100);
        }




}
void MainWindow::error(QAbstractSocket::SocketError)
{
    qDebug()<<"error!";
}
void MainWindow::connected()
{
    qDebug()<<"2";
    timer = new QTimer(this);
    qDebug()<<"3";
    connect(timer, SIGNAL(timeout()),this, SLOT(update()));
    qDebug()<<"6";
    timer ->start(100);
    qDebug()<<"7";
}

void MainWindow::disconnect()
{
    if(timer->isActive())
    {
        timer->stop();
    }
    ui->submitButton->setEnabled(true);
    ui->cancelButto->setEnabled(false);
    socket->close();
    socket = NULL;
}

void MainWindow::update()
{
    qDebug()<<"8";
    int ret = 0;
    char user_buff[20];
    char pass_buff[20];
    char recv_buff[20];
    char send_buff[30];
   // qDebug()<<"sendbuff1"<<strlen(send_buff);
    memset(user_buff, 0, sizeof(user_buff));
    memset(pass_buff, 0, sizeof(pass_buff));
    memset(recv_buff, 0, sizeof(recv_buff));
    memset(send_buff, 0, sizeof(send_buff));
    strcat(user_buff,"1|");
    strcat(pass_buff,"|");

    strcat(user_buff,ui->userLineEdit->text().toLatin1().data());
    strcat(pass_buff,ui->passwordLineEdit->text().toLatin1().data());

    strcat(user_buff,pass_buff);
    strcpy(send_buff, user_buff);
   // qDebug()<<"sendbuff2"<<strlen(send_buff);
   // qDebug()<<"send_buff"<<send_buff;
    ret = socket->write(send_buff,sizeof(send_buff));
    socket->waitForBytesWritten(3000);

     qDebug()<<"ret"<<ret;
   // socket->waitForReadyRead(200);
    if(ret != sizeof(send_buff))
    {
        qDebug()<<"failed1>>>>>>";
        socket->close();
    }
    socket->waitForReadyRead(200);


    ret = socket->read(recv_buff,sizeof(recv_buff));

    if(ret != sizeof(recv_buff))
    {
        timer->stop();
        socket->close();
        socket = NULL;
    }
    qDebug()<<"recv"<<ret;

   if( ret == 0)
    {
       qDebug()<<" recv failed2>>>>>>>";
       socket->close();
       qDebug()<<"11111111111111";
    }

    qDebug()<<"recv_buff"<<recv_buff;

   if(strcmp(recv_buff,"ok") == 0)

   {
       this->hide();
       LookWindow *q = new LookWindow;
       q->show();
       close();
       //socket->close();
       //socket = NULL;
        //return ;
   }

   else
    {
        QMessageBox::warning(this,"error!","please again!");
        ui->passwordLineEdit->clear();
        ui->userLineEdit->clear();
    }
     //socket->close();



}

void MainWindow::on_cancelButto_clicked()
{
    ui->passwordLineEdit->clear();
    ui->userLineEdit->clear();
   // if(timer->isActive())
     //     timer->stop();
       ui->submitButton->setEnabled(true);
       ui->cancelButto->setEnabled(false);

}




void MainWindow::on_specificationButton_clicked()
{
    this->hide();
    SpecificationWindow *q = new SpecificationWindow;
    q->show();
    close();
}
