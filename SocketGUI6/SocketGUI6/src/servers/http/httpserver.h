#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QObject>
#include <src/servers/tcp/tcpserver.h>
#include "httpconnection.h"

class HTTPServer : public TCPServer
{
    Q_OBJECT
public:
    explicit HTTPServer(QObject *parent = 0);

signals:

public slots:

    void accept(qintptr handle, TCPRunnable *runnable);
};

#endif // HTTPSERVER_H
