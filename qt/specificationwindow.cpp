#include "specificationwindow.h"
#include "ui_specificationwindow.h"

SpecificationWindow::SpecificationWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SpecificationWindow)
{
    ui->setupUi(this);
    this->setMaximumSize(350,280);
    this->setMinimumSize(350,280);



}

SpecificationWindow::~SpecificationWindow()
{
    delete ui;
}

void SpecificationWindow::on_returnButton_clicked()
{
    this->hide();
    MainWindow *q = new MainWindow;
    q->show();
    close();
}
