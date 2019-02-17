#include "httpserver.h"

HTTPServer::HTTPServer(QObject *parent) : TCPServer(parent)
{
    qDebug() << this << "created";
}

void HTTPServer::accept(qintptr handle, TCPRunnable *runnable)
{
    qDebug() << this << "Called in HTTP Server";
    qDebug() << this << "accepting" << handle << "on" << runnable;

    HTTPConnection *connection = new HTTPConnection;

    if(!connection)
    {
        qCritical() << this << "could not find connection to accept connection: " << handle;
        return;
    }

    qDebug() << this << "accepting" << handle;
    emit connecting(handle,runnable,connection);
}
