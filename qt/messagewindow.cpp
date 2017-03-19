#include "messagewindow.h"
#include "ui_messagewindow.h"

MessageWindow::MessageWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MessageWindow)
{
    ui->setupUi(this);
    this->setMaximumSize(400,250);
    this->setMinimumSize(400,250);
}

MessageWindow::~MessageWindow()
{
    delete ui;
}

void MessageWindow::on_entryButton_clicked()
{
    this->hide();
    LookWindow *p = new LookWindow;
    p->show();
    close();
}

void MessageWindow::on_cancelButton_clicked()
{

}
