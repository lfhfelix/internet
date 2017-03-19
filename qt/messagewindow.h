#ifndef MESSAGEWINDOW_H
#define MESSAGEWINDOW_H
#include"mainwindow.h"
#include"lookwindow.h"

#include <QWidget>

namespace Ui {
class MessageWindow;
}

class MessageWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MessageWindow(QWidget *parent = 0);
    ~MessageWindow();

private slots:
    void on_entryButton_clicked();

    void on_cancelButton_clicked();

private:
    Ui::MessageWindow *ui;
};

#endif // MESSAGEWINDOW_H
