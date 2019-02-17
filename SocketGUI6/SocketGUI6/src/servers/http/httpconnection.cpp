#include "httpconnection.h"

HTTPConnection::HTTPConnection(QObject *parent) : TCPConnection(parent)
{
    qDebug() << this << "created";
}

void HTTPConnection::readyRead()
{
    TCPConnection::readyRead();

    QTcpSocket *socket = static_cast<QTcpSocket*>(sender());
    if(!socket) return;

    QByteArray data = socket->readAll();
    qDebug() << this << socket << " bytes read = " << data.length();
    qDebug() << "data = " << data;

    //fake an HTTP server for Seige testing

    QByteArray response;

    QString page = "hello world";
    response.append("HTTP/1.1 200 OK\r\n");
    response.append("Content-Type: text/plain\r\n");
    response.append("Content-Length: " + QString::number(page.length()) + "\r\n");
    response.append("Connection: close\r\n\r\n");
    response.append(page);

    socket->write(response);
    socket->waitForBytesWritten();
    socket->close();
    qDebug() << this << "Http connection finished";

}
