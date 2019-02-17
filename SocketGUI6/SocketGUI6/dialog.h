#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

#include <src/servers/http/httpserver.h>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

private slots:
    void on_btnListen_clicked();

    void on_btnClose_clicked();

private:
    Ui::Dialog *ui;
    //TCPServer server;
    HTTPServer server;
};

#endif // DIALOG_H
