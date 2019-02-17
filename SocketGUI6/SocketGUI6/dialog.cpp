#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);


    QStringList lst = server.getAddresses();
    ui->cmbAddress->insertItems(0,lst);
    ui->cmbAddress->insertItem(0,"Any");
    ui->cmbAddress->setCurrentIndex(0);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_btnListen_clicked()
{
    //server.setMode(ui->comboBox->currentIndex());
    //server.setMaxThreads(2);

    server.setMode(TCPServer::MODE_SINGLE);

    if(ui->rdoSingle->isChecked()) server.setMode(TCPServer::MODE_SINGLE);
    if(ui->rdoPooled->isChecked()) server.setMode(TCPServer::MODE_POOLED);
    if(ui->rdoTheaded->isChecked()) server.setMode(TCPServer::MODE_THREADED);

    server.setMaxConnections(ui->spinBoxMax->value());
    server.setConnectionTimeout(ui->spinboxIdle->value());

    QHostAddress adr = QHostAddress::Any;
    if(ui->cmbAddress->currentIndex() > 0)
    {
        adr.setAddress(ui->cmbAddress->currentText());
    }

    qDebug() << "Starting Server on " << adr.toString() << ":" << ui->spinBox->value();

   server.listen(adr, ui->spinBox->value());


}

void Dialog::on_btnClose_clicked()
{
    server.close();
}
